/***************************************************************************
    copyright            : (C) 2008 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "filtermodel.h"
#include "models.h"
#include "../filter.h"
#include "../document.h"
#include "../collection.h"
#include "../entry.h"
#include "../tellico_debug.h"

#include <klocale.h>
#include <kicon.h>

using Tellico::FilterModel;

class FilterModel::Node {
public:
  Node(Node* parent_, int id_=-1) : m_parent(parent_), m_id(id_) {}
  ~Node() { qDeleteAll(m_children); }

  Node* parent() const { return m_parent; }
  Node* child(int row) const { return m_children.at(row); }
  int row() const { return m_parent ? m_parent->m_children.indexOf(const_cast<Node*>(this)) : 0; }
  int id() const { return m_id; }
  int count() const { return m_children.count(); }

  void addChild(Node* child) {  m_children.append(child); }
  void replaceChild(int i, Node* child) {  m_children.replace(i, child); }
  void removeChild(int i) {  delete m_children.takeAt(i); }

private:
  Node* m_parent;
  QList<Node*> m_children;
  int m_id;
};

FilterModel::FilterModel(QObject* parent) : QAbstractItemModel(parent), m_rootNode(new Node(0)) {
}

FilterModel::~FilterModel() {
  delete m_rootNode;
  m_rootNode = 0;
}

int FilterModel::rowCount(const QModelIndex& index_) const {
  if(!index_.isValid()) {
    return m_filters.count();
  }
  QModelIndex parent = index_.parent();
  if(parent.isValid()) {
    return 0; // a parent index means it points to an entry, not a filter, so there are no children
  }
  Node* node = static_cast<Node*>(index_.internalPointer());
  return node ? node->count() : 0;
}

int FilterModel::columnCount(const QModelIndex&) const {
  return 1;
}

QVariant FilterModel::headerData(int section_, Qt::Orientation orientation_, int role_) const {
  if(section_ >= columnCount() || orientation_ != Qt::Horizontal) {
    return QVariant();
  }
  if(role_ == Qt::DisplayRole) {
    return m_header;
  }
  return QVariant();
}

bool FilterModel::setHeaderData(int section_, Qt::Orientation orientation_,
                                    const QVariant& value_, int role_) {
  if(section_ >= columnCount() || orientation_ != Qt::Horizontal || role_ != Qt::EditRole) {
    return false;
  }
  m_header = value_.toString();
  emit headerDataChanged(orientation_, section_, section_);
  return true;
}

QVariant FilterModel::data(const QModelIndex& index_, int role_) const {
  if(!index_.isValid()) {
    return QVariant();
  }

  QModelIndex parent = index_.parent();

  if(index_.row() >= rowCount(parent)) {
    return QVariant();
  }

  switch(role_) {
    case Qt::DisplayRole:
      if(parent.isValid()) {
        // it points to an entry
        return entry(index_)->title();
      }
      // it points to a filter
      return filter(index_)->name();
    case Qt::DecorationRole:
      return parent.isValid() ? KIcon(entry(index_)->collection()->typeName())
                              : KIcon(QLatin1String("view-filter"));
    case RowCountRole:
      return rowCount(index_);
    case EntryPtrRole:
      return qVariantFromValue(entry(index_));
  }

  return QVariant();
}

QModelIndex FilterModel::index(int row_, int column_, const QModelIndex& parent_) const {
  if(!hasIndex(row_, column_, parent_)) {
    return QModelIndex();
  }

  Node* parentNode;
  if(parent_.isValid()) {
    parentNode = static_cast<Node*>(parent_.internalPointer());
  } else {
    parentNode = m_rootNode;
  }

  Node* child = parentNode->child(row_);
  if(!child) {
    return QModelIndex();
  }
  return createIndex(row_, column_, child);
}

QModelIndex FilterModel::parent(const QModelIndex& index_) const {
  if(!index_.isValid()) {
    return QModelIndex();
  }

  Node* node = static_cast<Node*>(index_.internalPointer());
  Node* parentNode = node->parent();

  // if it's top-level, it has no parent
  if(parentNode == m_rootNode) {
    return QModelIndex();
  }
  return createIndex(parentNode->row(), 0, parentNode);
}

void FilterModel::clear() {
  m_filters.clear();
  delete m_rootNode;
  m_rootNode = new Node(0);
  reset();
}

void FilterModel::addFilters(const Tellico::FilterList& filters_) {
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+filters_.count()-1);
  m_filters += filters_;
  foreach(FilterPtr filter, filters_) {
    Node* filterNode = new Node(m_rootNode);
    m_rootNode->addChild(filterNode);
    Data::EntryList entries = Data::Document::self()->filteredEntries(filter);
    foreach(Data::EntryPtr entry, entries) {
      Node* childNode = new Node(filterNode, entry->id());
      filterNode->addChild(childNode);
    }
  }
  endInsertRows();
}

QModelIndex FilterModel::addFilter(Tellico::FilterPtr filter_) {
  Q_ASSERT(filter_);
  addFilters(FilterList() << filter_);
  // rowCount() has increased now
  return index(rowCount()-1, 0);
}

void FilterModel::removeFilter(Tellico::FilterPtr filter_) {
  Q_ASSERT(filter_);
  int idx = m_filters.indexOf(filter_);
  if(idx < 0) {
    myWarning() << "no filter named" << filter_->name();
    return;
  }

  beginRemoveRows(QModelIndex(), idx, idx);
  m_filters.removeAt(idx);
  m_rootNode->removeChild(idx);
  endRemoveRows();
}

Tellico::FilterPtr FilterModel::filter(const QModelIndex& index_) const {
  // if the parent isn't invalid, then it's not a top-level filter
  if(!index_.isValid() || index_.parent().isValid() || index_.row() >= m_filters.count()) {
    return FilterPtr();
  }
  return m_filters.at(index_.row());
}

Tellico::Data::EntryPtr FilterModel::entry(const QModelIndex& index_) const {
  // if there's not a parent, then it's a top-level item, no entry
  if(!index_.parent().isValid()) {
    return Data::EntryPtr();
  }
  Data::EntryPtr entry;
  Node* node = static_cast<Node*>(index_.internalPointer());
  if(node) {
    entry = Data::Document::self()->collection()->entryById(node->id());
  }
  return entry;
}

void FilterModel::invalidate(const QModelIndex& index_) {
  // delete and recreate the node, only if
  // it has no parent, i.e. it points to a filter
  if(index_.parent().isValid()) {
    return;
  }
  emit layoutAboutToBeChanged();
  Node* oldNode = static_cast<Node*>(index_.internalPointer());
  Node* newNode = new Node(m_rootNode);
  m_rootNode->replaceChild(index_.row(), newNode);
  Data::EntryList entries = Data::Document::self()->filteredEntries(filter(index_));
  foreach(Data::EntryPtr entry, entries) {
    Node* childNode = new Node(newNode, entry->id());
    newNode->addChild(childNode);
  }
  delete oldNode;
  emit layoutChanged();
}

#include "filtermodel.moc"