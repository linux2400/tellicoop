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

#undef QT_NO_CAST_FROM_ASCII

#include "qtest_kde.h"
#include "risimporttest.h"
#include "risimporttest.moc"

#include "../translators/risimporter.h"
#include "../collection.h"

QTEST_KDEMAIN_CORE( RisImportTest )

void RisImportTest::testEmpty() {
  KUrl::List emptyList;
  Tellico::Import::RISImporter importer(emptyList);
  Tellico::Data::CollPtr coll = importer.collection();

  QVERIFY(!coll.isNull());
  QCOMPARE(coll->entryCount(), 0);
}