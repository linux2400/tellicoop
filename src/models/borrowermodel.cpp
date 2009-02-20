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

#include "borrowermodel.h"
#include "models.h"
#include "../document.h"
#include "../collection.h"
#include "../entry.h"
#include "../tellico_debug.h"

#include <klocale.h>
#include <kicon.h>

using Tellico::BorrowerModel;

class BorrowerModel::Node {
public:
  Node(Node* parent_) : m_parent(parent_) {}
  ~Node() { qDeleteAll(m_children); }

  Node* parent() const { return m_parent; }
  Node* child(int row) const { return m_children.at(row); }
  int row() const { return m_parent ? m_parent->m_children.indexOf(const_cast<Node*>(this)) : 0; }
  int count() const { return m_children.count(); }

  void addChild(Node* child) {  m_children.append(child); }
  void replaceChild(int i, Node* child) {  m_children.replace(i, child); }
  void removeChild(int i) {  delete m_children.takeAt(i); }

private:
  Node* m_parent;
  QList<Node*> m_children;
};

BorrowerModel::BorrowerModel(QObject* parent) : QAbstractItemModel(parent), m_rootNode(new Node(0)) {
}

BorrowerModel::~BorrowerModel() {
  delete m_rootNode;
  m_rootNode = 0;
}

int BorrowerModel::rowCount(const QModelIndex& index_) const {
  if(!index_.isValid()) {
    return m_borrowers.count();
  }
  QModelIndex parent = index_.parent();
  if(parent.isValid()) {
    return 0; // a parent index means it points to an entry, not a filter, so there are no children
  }
  Node* node = static_cast<Node*>(index_.internalPointer());
  return node ? node->count() : 0;
}

int BorrowerModel::columnCount(const QModelIndex&) const {
  return 1;
}

QVariant BorrowerModel::headerData(int section_, Qt::Orientation orientation_, int role_) const {
  if(section_ >= columnCount() || orientation_ != Qt::Horizontal) {
    return QVariant();
  }
  if(role_ == Qt::DisplayRole) {
    return m_header;
  }
  return QVariant();
}

bool BorrowerModel::setHeaderData(int section_, Qt::Orientation orientation_,
                                  const QVariant& value_, int role_) {
  if(section_ >= columnCount() || orientation_ != Qt::Horizontal || role_ != Qt::EditRole) {
    return false;
  }
  m_header = value_.toString();
  emit headerDataChanged(orientation_, section_, section_);
  return true;
}

QVariant BorrowerModel::data(const QModelIndex& index_, int role_) const {
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
      // it points to a borrower
      return borrower(index_)->name();
    case Qt::DecorationRole:
      return parent.isValid() ? KIcon(entry(index_)->collection()->typeName())
                              : KIcon(QLatin1String("kaddressbook"));
    case RowCountRole:
      return rowCount(index_);
    case EntryPtrRole:
      return qVariantFromValue(entry(index_));
  }

  return QVariant();
}

QModelIndex BorrowerModel::index(int row_, int column_, const QModelIndex& parent_) const {
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

QModelIndex BorrowerModel::parent(const QModelIndex& index_) const {
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

void BorrowerModel::clear() {
  m_borrowers.clear();
  delete m_rootNode;
  m_rootNode = new Node(0);
  reset();
}

void BorrowerModel::addBorrowers(const Tellico::Data::BorrowerList& borrowers_) {
  beginInsertRows(QModelIndex(), rowCount(), rowCount()+borrowers_.count()-1);
  m_borrowers += borrowers_;
  foreach(Data::BorrowerPtr borrower, borrowers_) {
    Node* borrowerNode = new Node(m_rootNode);
    m_rootNode->addChild(borrowerNode);
    for(int i = 0; i < borrower->count(); ++i) {
      Node* childNode = new Node(borrowerNode);
      borrowerNode->addChild(childNode);
    }
  }
  endInsertRows();
}

QModelIndex BorrowerModel::addBorrower(Tellico::Data::BorrowerPtr borrower_) {
  Q_ASSERT(borrower_);
  addBorrowers(Data::BorrowerList() << borrower_);
  // rowCount() has increased now
  return index(rowCount()-1, 0);
}

QModelIndex BorrowerModel::modifyBorrower(Tellico::Data::BorrowerPtr borrower_) {
  Q_ASSERT(borrower_);
  int idx = m_borrowers.indexOf(borrower_);
  if(idx < 0) {
    myWarning() << "no borrower named" << borrower_->name();
    return QModelIndex();
  }

  emit layoutAboutToBeChanged();
  Node* oldNode = m_rootNode->child(idx);
  Node* newNode = new Node(m_rootNode);
  m_rootNode->replaceChild(idx, newNode);
  for(int i = 0; i < borrower_->count(); ++i) {
    Node* childNode = new Node(newNode);
    newNode->addChild(childNode);
  }
  delete oldNode;

  emit layoutChanged();
  return index(idx, 0);
}

void BorrowerModel::removeBorrower(Tellico::Data::BorrowerPtr borrower_) {
  Q_ASSERT(borrower_);
  int idx = m_borrowers.indexOf(borrower_);
  if(idx < 0) {
    myWarning() << "no borrower named" << borrower_->name();
    return;
  }

  beginRemoveRows(QModelIndex(), idx, idx);
  m_borrowers.removeAt(idx);
  m_rootNode->removeChild(idx);
  endRemoveRows();
}

Tellico::Data::BorrowerPtr BorrowerModel::borrower(const QModelIndex& index_) const {
  // if the parent isn't invalid, then it's not a top-level borrower
  if(!index_.isValid() || index_.parent().isValid() || index_.row() >= m_borrowers.count()) {
    return Data::BorrowerPtr();
  }
  return m_borrowers.at(index_.row());
}

Tellico::Data::EntryPtr BorrowerModel::entry(const QModelIndex& index_) const {
  // if there's not a parent, then it's a top-level item, no entry
  if(!index_.parent().isValid()) {
    return Data::EntryPtr();
  }
  Data::EntryPtr entry;
  Data::LoanPtr loan = this->loan(index_);
  if(loan) {
    entry = loan->entry();
  }
  return entry;
}

Tellico::Data::LoanPtr BorrowerModel::loan(const QModelIndex& index_) const {
  // if there's not a parent, then it's a top-level item, no entry
  if(!index_.parent().isValid()) {
    return Data::LoanPtr();
  }
  Data::LoanPtr loan;
  Data::BorrowerPtr borrower = this->borrower(index_.parent());
  if(borrower) {
    loan = borrower->loans().at(index_.row());
  }
  return loan;
}

#include "borrowermodel.moc"