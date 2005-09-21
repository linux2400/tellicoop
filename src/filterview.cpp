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

#include "filterview.h"
#include "controller.h"
#include "entry.h"
#include "collection.h"
#include "document.h"
#include "entryitem.h"
#include "tellico_kernel.h"

#include <klocale.h>
#include <kpopupmenu.h>
#include <kiconloader.h>
#include <kdebug.h>

#include <qheader.h>

using Tellico::FilterView;

FilterView::FilterView(QWidget* parent_, const char* name_) : GUI::ListView(parent_, name_), m_notSortedYet(true) {
  addColumn(i18n("Filter"));
  header()->setStretchEnabled(true, 0);
  setResizeMode(QListView::NoColumn);
  setRootIsDecorated(true);
  setShowSortIndicator(true);
  setTreeStepSize(15);
  setFullWidth(true);

  m_filterMenu = new KPopupMenu(this);
  m_filterMenu->insertItem(SmallIconSet(QString::fromLatin1("filter")),
                           i18n("Modify Filter"), this, SLOT(slotModifyFilter()));
  m_filterMenu->insertItem(SmallIconSet(QString::fromLatin1("editdelete")),
                           i18n("Delete Filter"), this, SLOT(slotDeleteFilter()));

  connect(this, SIGNAL(contextMenuRequested(QListViewItem*, const QPoint&, int)),
          SLOT(contextMenuRequested(QListViewItem*, const QPoint&, int)));
}

bool FilterView::isSelectable(GUI::ListViewItem* item_) const {
  if(!GUI::ListView::isSelectable(item_)) {
    return false;
  }

  // because the popup menu has modify and delete, only
  // allow one filter item to get selected
  if(item_->isFilterItem()) {
    return selectedItems().isEmpty();
  }

  return true;
}

void FilterView::contextMenuRequested(QListViewItem* item_, const QPoint& point_, int) {
  if(!item_) {
    return;
  }

  GUI::ListViewItem* item = static_cast<GUI::ListViewItem*>(item_);
  if(item->isFilterItem() && m_filterMenu->count() > 0) {
    m_filterMenu->popup(point_);
  }
}

// this gets called when header() is clicked, so cycle through
void FilterView::setSorting(int col_, bool asc_) {
  if(asc_ && !m_notSortedYet) {
    if(sortStyle() == ListView::SortByText) {
      setSortStyle(ListView::SortByCount);
    } else {
      setSortStyle(ListView::SortByText);
    }
  }
  if(sortStyle() == ListView::SortByText) {
    setColumnText(0, i18n("Filter"));
  } else {
    setColumnText(0, i18n("Filter (Sort by Count)"));
  }
  m_notSortedYet = false;
  ListView::setSorting(col_, asc_);
}

void FilterView::addCollection(Data::Collection* coll_) {
  FilterVec filters = coll_->filters();
  for(FilterVec::Iterator it = filters.begin(); it != filters.end(); ++it) {
    addFilter(it);
  }
}

void FilterView::addEntry(Data::Entry* entry_) {
  for(QListViewItem* item = firstChild(); item; item = item->nextSibling()) {
    Filter* filter = static_cast<FilterItem*>(item)->filter();
    if(filter && filter->matches(entry_)) {
      new EntryItem(static_cast<FilterItem*>(item), entry_);
    }
  }
}

void FilterView::modifyEntry(Data::Entry* entry_) {
  for(QListViewItem* item = firstChild(); item; item = item->nextSibling()) {
    bool hasEntry = false;
    QListViewItem* entryItem = 0;
    // iterate over all children and find item with matching entry pointers
    QListViewItem* i = item->firstChild();
    while(i) {
      if(static_cast<EntryItem*>(i)->entry() == entry_) {
        i->setText(0, entry_->title());
        // only one item per filter will match
        hasEntry = true;
        entryItem = i;
        break;
      }
      i = i->nextSibling();
    }
    // now, if the entry was there but no longer matches, delete it
    // if the entry was not there but does match, add it
    Filter* filter = static_cast<FilterItem*>(item)->filter();
    if(hasEntry && !filter->matches(static_cast<EntryItem*>(entryItem)->entry())) {
      delete entryItem;
    } else if(!hasEntry && filter->matches(entry_)) {
      new EntryItem(static_cast<FilterItem*>(item), entry_);
    }
  }
}

void FilterView::removeEntry(Data::Entry* entry_) {
  // the group modified signal gets handles separately, this is just for filters
  for(QListViewItem* item = firstChild(); item; item = item->nextSibling()) {
    // iterate over all children and delete items with matching entry pointers
    QListViewItem* i = item->firstChild();
    while(i) {
      if(static_cast<EntryItem*>(i)->entry() == entry_) {
        delete i;
        i = 0;
        // only one item per filter will match
        break;
      }
      i = i->nextSibling();
    }
  }
}
void FilterView::addFilter(Filter* filter_) {
  FilterItem* filterItem = new FilterItem(this, filter_);

  Data::EntryVec entries = Data::Document::self()->filteredEntries(filter_);
  for(Data::EntryVecIt it = entries.begin(); it != entries.end(); ++it) {
    new EntryItem(filterItem, it); // text gets set in constructor
  }
}

void FilterView::slotModifyFilter() {
  GUI::ListViewItem* item = static_cast<GUI::ListViewItem*>(currentItem());
  if(!item || !item->isFilterItem()) {
    return;
  }

  Kernel::self()->modifyFilter(static_cast<FilterItem*>(item)->filter());
}

void FilterView::slotDeleteFilter() {
  GUI::ListViewItem* item = static_cast<GUI::ListViewItem*>(currentItem());
  if(!item || !item->isFilterItem()) {
    return;
  }

  Kernel::self()->removeFilter(static_cast<FilterItem*>(item)->filter());
}

void FilterView::removeFilter(Filter* filter_) {
  // paranoia
  if(!filter_) {
    return;
  }

  // find the item for this filter
  // cheating a bit, it's probably the current one
  GUI::ListViewItem* found = 0;
  GUI::ListViewItem* cur = static_cast<GUI::ListViewItem*>(currentItem());
  if(cur && cur->isFilterItem() && static_cast<FilterItem*>(cur)->filter() == filter_) {
    // found it!
    found = cur;
  } else {
    // iterate over all filter items
    for(QListViewItem* item = firstChild(); item; item = item->nextSibling()) {
      if(static_cast<FilterItem*>(item)->filter() == filter_) {
        found = static_cast<FilterItem*>(item);
        break;
      }
    }
  }

  // not found
  if(!found) {
    kdDebug() << "GroupView::modifyFilter() - not found" << endl;
    return;
  }

  delete found;
}

#include "filterview.moc"