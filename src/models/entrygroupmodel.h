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

#ifndef TELLICO_ENTRYGROUPMODEL_H
#define TELLICO_ENTRYGROUPMODEL_H

#include "../entry.h"

#include <QAbstractItemModel>

namespace Tellico {
  namespace Data {
   class EntryGroup;
  }

/**
 * @author Robby Stephenson
 */
class EntryGroupModel : public QAbstractItemModel {
Q_OBJECT

public:
  EntryGroupModel(QObject* parent);
  virtual ~EntryGroupModel();

  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role=Qt::EditRole);
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
  virtual QModelIndex index(int row, int column=0, const QModelIndex& parent = QModelIndex()) const;
  virtual QModelIndex parent(const QModelIndex& index) const;

  void clear();
  void addGroups(const QList<Data::EntryGroup*>& groups, const QString& iconName);
  QModelIndex addGroup(Data::EntryGroup* group);
  QModelIndex modifyGroup(Data::EntryGroup* group);
  void removeGroup(Data::EntryGroup* group);

  Data::EntryGroup* group(const QModelIndex& index) const;
  Data::EntryPtr entry(const QModelIndex& index) const;
  QModelIndex indexFromGroup(Data::EntryGroup* group) const;

private:
  QList<Data::EntryGroup*> m_groups;
  class Node;
  Node* m_rootNode;
  QStringList m_groupIconNames;
  QString m_groupHeader;
};

} // end namespace
#endif