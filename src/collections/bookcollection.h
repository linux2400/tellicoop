/***************************************************************************
    copyright            : (C) 2003-2004 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifndef BOOKCOLLECTION_H
#define BOOKCOLLECTION_H

#include "../collection.h"

#include <klocale.h>

namespace Bookcase {
  namespace Data {

/**
 * A BCCollection for books.
 *
 * It has the following standard attributes:
 * @li Title
 * @li Subtitle
 * @li Author
 * @li Binding
 * @li Purchase Date
 * @li Purchase Price
 * @li Publisher
 * @li Edition
 * @li Copyright Year
 * @li Publication Year
 * @li ISBN Number
 * @li Library of Congress Catalog Number
 * @li Pages
 * @li Language
 * @li Genre
 * @li Keywords
 * @li Series
 * @li Series Number
 * @li Condition
 * @li Signed
 * @li Read
 * @li Gift
 * @li Loaned
 * @li Rating
 * @li Comments
 *
 * @author Robby Stephenson
 * @version $Id: bookcollection.h 386 2004-01-24 05:12:28Z robby $
 */
class BookCollection : public Collection {
Q_OBJECT

public: 
  /**
   * The constructor
   *
   * @param addFields Whether to add the default attributes
   * @param title The title of the collection
   */
  BookCollection(bool addFields, const QString& title = QString::null);

  virtual CollectionType collectionType() const { return Book; }

  static FieldList defaultFields();
};

  } // end namespace
} // end namespace
#endif