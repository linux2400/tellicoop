/***************************************************************************
    copyright            : (C) 2005-2006 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifndef TELLICO_DATA_FILECATALOG_H
#define TELLICO_DATA_FILECATALOG_H

#include "../collection.h"

namespace Tellico {
  namespace Data {

/**
 * @author Robby Stephenson
 */
class FileCatalog : public Collection {
Q_OBJECT

public:
  FileCatalog(bool addFields, const QString& title = QString::null);

  virtual Type type() const { return File; }

  static FieldVec defaultFields();
};

  } // end namespace
} // end namespace
#endif