/***************************************************************************
    copyright            : (C) 2009 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifndef TELLICO_ENTRYCOMPARISON_H
#define TELLICO_ENTRYCOMPARISON_H

#include "datavectors.h"

namespace Tellico {

class EntryComparison {
public:
  static int score(Data::EntryPtr entry1, Data::EntryPtr entry2, Data::FieldPtr field);
  static int score(Data::EntryPtr entry1, Data::EntryPtr entry2, const QString& field, const Data::Collection* coll);
};

} // namespace
#endif