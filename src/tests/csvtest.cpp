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

#include "qtest_kde.h"
#include "csvtest.h"
#include "csvtest.moc"
#include "../translators/csvparser.h"

QTEST_MAIN( CsvTest )

void CsvTest::initTestCase() {
  QString dummy;
  p = new Tellico::CSVParser(dummy);
  p->setDelimiter(QChar(','));
}

void CsvTest::cleanupTestCase() {
  delete p;
}

void CsvTest::init() {
  QFETCH(QString, delim);
  p->setDelimiter(delim);
}

void CsvTest::testAll() {
  QFETCH(QString, line);
  QFETCH(int, pos1);
  QFETCH(QString, token1);
  QFETCH(int, pos2);
  QFETCH(QString, token2);

  p->reset(line);
  QStringList tokens = p->nextTokens();

  QTEST(tokens.count(), "count");
  QTEST(tokens[pos1], "token1");
  QTEST(tokens[pos2], "token2");
}

void CsvTest::testAll_data() {
  QTest::addColumn<QString>("line");
  QTest::addColumn<QString>("delim");
  QTest::addColumn<int>("count");
  QTest::addColumn<int>("pos1");
  QTest::addColumn<QString>("token1");
  QTest::addColumn<int>("pos2");
  QTest::addColumn<QString>("token2");

  QTest::newRow("basic") << "robby,stephenson is cool\t,," << "," << 4 << 0 << "robby" << 2 << "";
  QTest::newRow("space") << "robby,stephenson is cool\t,," << " " << 3 << 0 << "robby,stephenson" << 2 << "cool\t,,";
  QTest::newRow("tab") << "robby\t\tstephenson" << "\t" << 3 << 0 << "robby" << 1 << "";
  // quotes get swallowed
  QTest::newRow("quotes") << "robby,\"stephenson,is,cool\"" << "," << 2 << 0 << "robby" << 1 << "stephenson,is,cool";
}