/***************************************************************************
    copyright            : (C) 2001-2004 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifndef BCDETAILEDLISTVIEW_H
#define BCDETAILEDLISTVIEW_H

// needed for EntryList definition
#include "entry.h"

#include <klistview.h>
#include <kpopupmenu.h>

#include <qpoint.h>
#include <qstringlist.h>
#include <qpixmap.h>
#include <qvaluevector.h>
#include <qguardedptr.h>

namespace Bookcase {
  namespace Data {
    class Collection;
  }
  class Filter;
  class EntryItem;

/**
 * The DetailedListView class shows detailed information about units in the
 * collection.
 *
 * @author Robby Stephenson
 * @version $Id: detailedlistview.h 407 2004-01-29 03:11:43Z root $
 */
class DetailedListView : public KListView {
Q_OBJECT

public:
  /**
   * The constructor initializes the popup menu, but no columns are inserted.
   *
   * @param parent A pointer to the parent widget
   * @param name The widget name
   */
  DetailedListView(QWidget* parent, const char* name=0);
  ~DetailedListView();
  
  /**
   * Event filter used to popup the menu
   */
  bool eventFilter(QObject* obj, QEvent* ev);
  /**
   * Clears the selection.
   */
  void clearSelection();
  /**
   * Selects the item which refers to a certain entry.
   *
   * @param entry A pointer to the entry
   */
  void setEntrySelected(Data::Entry* entry);
  void setFilter(const Filter* filter);
  const Filter* filter() const;
  
  int prevSortedColumn() const;
  int prev2SortedColumn() const;
  virtual void setSorting(int column, bool ascending = true);
  void setPrevSortedColumn(int prev1, int prev2);
  QString sortColumnTitle1() const;
  QString sortColumnTitle2() const;
  QString sortColumnTitle3() const;
  QStringList visibleColumns() const;
  Data::EntryList visibleEntries();
  /**
   * Returns whether the given column is formatted as a number or not.
   *
   * @param column Column number
   * @return Whether column is formatted as a number or not
   */
  bool isNumber(int column) const;
  /**
   * @param coll A pointer to the collection
   */
  void addCollection(Bookcase::Data::Collection* coll);
  /**
   * Removes all items which refers to a entry within a collection.
   *
   * @param coll A pointer to the collection
   */
  void removeCollection(Bookcase::Data::Collection* coll);
  /**
   * Adds a new list item showing the details for a entry.
   *
   * @param entry A pointer to the entry
   */
  void addEntry(Data::Entry* entry);
  /**
   * Modifies any item which refers to a entry, resetting the column contents.
   *
   * @param entry A pointer to the entry
   */
  void modifyEntry(Data::Entry* entry);
  /**
   * Removes any item which refers to a certain entry.
   *
   * @param entry A pointer to the entry
   */
  void removeEntry(Data::Entry* entry);
  void addField(Data::Field* field, int width);
  void modifyField(Data::Field* newField, Data::Field* oldField);
  void removeField(Data::Field* field);
  void reorderFields(const Data::FieldList& list);
  void saveConfig(Bookcase::Data::Collection* coll);
  /**
   * Select all visible items.
   */
  void selectAllVisible();

public slots:
  /**
   * Resets the list view, clearing and deleting all items.
   */
  void slotReset();
  /**
   * Refreshes the view, repopulating all items.
   */
  void slotRefresh();

protected:
  /**
   * A helper method to populate an item. The column text is initialized by pulling
   * the contents from the entry pointer of the item, so it should properly be set
   * before this method is called.
   *
   * @param item A pointer to the item
   */
  void populateItem(EntryItem* item);
  void populateColumn(int col);
  void setPixmapAndText(EntryItem* item, int col, Data::Field* field);
  
  /**
   * A helper method to locate any item which refers to a certain entry. If none
   * is found, a NULL pointer is returned.
   *
   * @param entry A pointer to the entry
   * @return A pointer to the item
   */
  EntryItem* const locateItem(const Data::Entry* entry);
  void showColumn(int col);
  void hideColumn(int col);
  void updateFirstSection();

protected slots:
  /**
   * Handles the appearance of the popup menu.
   *
   * @param item A pointer to the list item underneath the mouse
   * @param point The location point
   * @param col The column number, not currently used
   */
  void slotRMB(QListViewItem* item, const QPoint& point, int col);
  /**
   * Handles everything when an item is selected. The proper signal is emitted.
   */
  void slotSelectionChanged();
  void slotHeaderMenuActivated(int id);
  void slotCacheColumnWidth(int section, int oldSize, int newSize);
  /**
   * Slot to update the position of the pixmap
   */
  void slotUpdatePixmap();

signals:
  /**
   * Signals that the selected units have changed. Zero, one or more may be selected.
   *
   * @param widget A pointer to the widget where the selection changed, this widget
   * @param list A list of the selected items, may be empty.
   */
  void signalEntrySelected(QWidget* widget, const Bookcase::Data::EntryList& list);
  /**
   * Signals a desire to delete a entry.
   *
   * @param entry A pointer to the entry
   */
  void signalDeleteEntry(Bookcase::Data::Entry* entry);
  /**
   * Signals that a fraction of an operation has been completed.
   *
   * @param f The fraction, 0 =< f >= 1
   */
  void signalFractionDone(float f);

private:
  KPopupMenu* m_itemMenu;
  KPopupMenu* m_headerMenu;
  QValueVector<int> m_columnWidths;
  QValueVector<bool> m_isNumber;
  QValueVector<bool> m_isDirty;
  QPixmap m_entryPix;
  QPixmap m_checkPix;
  Data::EntryList m_selectedEntries;
  const Filter* m_filter;
  int m_prevSortColumn;
  int m_prev2SortColumn;
  int m_firstSection;
};

} // end namespace;
#endif