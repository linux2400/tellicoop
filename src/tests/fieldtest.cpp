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
#include "fieldtest.h"
#include "fieldtest.moc"

#include "../field.h"

QTEST_KDEMAIN_CORE( FieldTest )

void FieldTest::testEmpty() {
  Tellico::Data::Field field1("name", "title");

  // the default field type is line
  QCOMPARE(field1.type(), Tellico::Data::Field::Line);
  QCOMPARE(field1.name(), QLatin1String("name"));
  QCOMPARE(field1.title(), QLatin1String("title"));
  // description should match title, to begin with
  QCOMPARE(field1.description(), QLatin1String("title"));
  QCOMPARE(field1.id(), (long)1);
  QCOMPARE(field1.flags(), 0);
  QCOMPARE(field1.formatFlag(), Tellico::Data::Field::FormatNone);
  field1.setTitle("newtitle");
  QCOMPARE(field1.title(), QLatin1String("newtitle"));
  field1.setCategory("category");
  QCOMPARE(field1.category(), QLatin1String("category"));
  field1.setDefaultValue("default");
  QCOMPARE(field1.defaultValue(), QLatin1String("default"));
  QCOMPARE(field1.isSingleCategory(), false);

  Tellico::Data::Field field1a = field1;
  QCOMPARE(field1.type(), field1a.type());
  QCOMPARE(field1.category(), field1a.category());
  QCOMPARE(field1.propertyList(), field1a.propertyList());

  Tellico::Data::Field field2("table", "Table", Tellico::Data::Field::Table2);
  // should be converted to Table type
  QCOMPARE(field2.type(), Tellico::Data::Field::Table);
  QCOMPARE(field2.property("columns"), QLatin1String("2"));
  field2.setProperty("columns", "1");
  QCOMPARE(field2.property("columns"), QLatin1String("1"));
  QCOMPARE(field2.isSingleCategory(), true);

  QStringList allowed;
  allowed << "choice1";
  Tellico::Data::Field field3("choice", "Choice", allowed);
  QCOMPARE(field3.type(), Tellico::Data::Field::Choice);
  QCOMPARE(field3.allowed(), allowed);
  field3.setDefaultValue("default");
  QCOMPARE(field3.defaultValue(), QLatin1String(""));
}