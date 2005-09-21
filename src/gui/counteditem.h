/***************************************************************************
    copyright            : (C) 2005 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifndef COUNTEDITEM_H
#define COUNTEDITEM_H

class QPainter;
class QColorGroup;
class QFontMetrics;

#include "listview.h"

namespace Tellico {
  namespace GUI {

/**
 * @author Robby Stephenson
 */
class CountedItem : public GUI::ListViewItem {
public:
  CountedItem(ListView* parent) : ListViewItem(parent) {}
  CountedItem(ListViewItem* parent) : ListViewItem(parent) {}

  virtual int compare(QListViewItem* item, int col, bool ascending) const;
  /**
   * Paints the cell, adding the number count.
   */
  virtual void paintCell(QPainter* p, const QColorGroup& cg,
                         int column, int width, int align);
  virtual int width(const QFontMetrics& fm, const QListView* lv, int c) const;
};

  } // end namespace
} // end namespace

#endif