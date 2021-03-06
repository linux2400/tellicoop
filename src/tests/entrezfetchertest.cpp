/***************************************************************************
    Copyright (C) 2013 Robby Stephenson <robby@periapsis.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#undef QT_NO_CAST_FROM_ASCII

#include "entrezfetchertest.h"
#include "entrezfetchertest.moc"
#include "qtest_kde.h"

#include "../fetch/entrezfetcher.h"
#include "../entry.h"
#include "../collections/bibtexcollection.h"
#include "../collectionfactory.h"

#include <KStandardDirs>
#include <KConfigGroup>

QTEST_KDEMAIN( EntrezFetcherTest, GUI )

EntrezFetcherTest::EntrezFetcherTest() : AbstractFetcherTest() {
}

void EntrezFetcherTest::initTestCase() {
  KGlobal::dirs()->addResourceDir("appdata", QString::fromLatin1(KDESRCDIR) + "/../../xslt/");
  Tellico::RegisterCollection<Tellico::Data::BibtexCollection> registerBibtex(Tellico::Data::Collection::Bibtex, "bibtex");

  m_fieldValues.insert(QLatin1String("doi"), QLatin1String("10.1016/j.tcb.2013.03.001"));
  m_fieldValues.insert(QLatin1String("pmid"), QLatin1String("23597843"));
  m_fieldValues.insert(QLatin1String("title"), QLatin1String("Stem cell competition: finding balance in the niche."));
  m_fieldValues.insert(QLatin1String("author"), QString::fromUtf8("Rachel R Stine; Erika L Matunis"));
//  m_fieldValues.insert(QLatin1String("volume"), QLatin1String("85"));
  m_fieldValues.insert(QLatin1String("journal"), QLatin1String("Trends in cell biology"));
//  m_fieldValues.insert(QLatin1String("publisher"), QLatin1String("Springer"));
  m_fieldValues.insert(QLatin1String("year"), QLatin1String("2013"));
  m_fieldValues.insert(QLatin1String("month"), QLatin1String("8"));
  m_fieldValues.insert(QLatin1String("entry-type"), QLatin1String("article"));
}

void EntrezFetcherTest::testTitle() {
  KConfig config(QString::fromLatin1(KDESRCDIR) + QLatin1String("/tellicotest.config"), KConfig::SimpleConfig);
  QString groupName = QLatin1String("entrez");
  if(!config.hasGroup(groupName)) {
    QSKIP("This test requires a config file.", SkipAll);
  }
  KConfigGroup cg(&config, groupName);

  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Bibtex, Tellico::Fetch::Title,
                                       m_fieldValues.value("title"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::EntrezFetcher(this));
  fetcher->readConfig(cg, cg.name());

  Tellico::Data::EntryList results = DO_FETCH1(fetcher, request, 1);

  QCOMPARE(results.size(), 1);
  Tellico::Data::EntryPtr entry = results.at(0);

  QHashIterator<QString, QString> i(m_fieldValues);
  while(i.hasNext()) {
    i.next();
    QCOMPARE(entry->field(i.key()), i.value());
  }
  QVERIFY(entry->field(QLatin1String("abstract")).contains(QLatin1String("Drosophila")));
}

void EntrezFetcherTest::testAuthor() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Bibtex, Tellico::Fetch::Person,
                                       QLatin1String("Rachel R Stine"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::EntrezFetcher(this));

  Tellico::Data::EntryList results = DO_FETCH(fetcher, request);

  QVERIFY(results.size() > 1);
}

void EntrezFetcherTest::testKeyword() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Bibtex, Tellico::Fetch::Keyword,
                                       QLatin1String("Drosophila"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::EntrezFetcher(this));

  // fetcher defaults to 25 at a time, ask for 26 to check search continue
  Tellico::Data::EntryList results = DO_FETCH1(fetcher, request, 26);
  QCOMPARE(results.size(), 26);

  // and make sure it's 26 different
  QSet<QString> titles;
  foreach(Tellico::Data::EntryPtr entry, results) {
    titles.insert(entry->title());
  }
  QCOMPARE(titles.size(), results.size());

}

void EntrezFetcherTest::testPMID() {
  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Bibtex, Tellico::Fetch::PubmedID,
                                       m_fieldValues.value("pmid"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::EntrezFetcher(this));

  // there are several results for the same ISBN here
  Tellico::Data::EntryList results = DO_FETCH1(fetcher, request, 1);

  QCOMPARE(results.size(), 1);
}

void EntrezFetcherTest::testDOI() {
  KConfig config(QString::fromLatin1(KDESRCDIR) + QLatin1String("/tellicotest.config"), KConfig::SimpleConfig);
  QString groupName = QLatin1String("entrez");
  if(!config.hasGroup(groupName)) {
    QSKIP("This test requires a config file.", SkipAll);
  }
  KConfigGroup cg(&config, groupName);

  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Bibtex, Tellico::Fetch::DOI,
                                       m_fieldValues.value("doi"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::EntrezFetcher(this));
  fetcher->readConfig(cg, cg.name());

  Tellico::Data::EntryList results = DO_FETCH1(fetcher, request, 1);

  QCOMPARE(results.size(), 1);
  Tellico::Data::EntryPtr entry = results.at(0);

  QHashIterator<QString, QString> i(m_fieldValues);
  while(i.hasNext()) {
    i.next();
    QCOMPARE(entry->field(i.key()), i.value());
  }
  QVERIFY(entry->field(QLatin1String("abstract")).contains(QLatin1String("Drosophila")));
}
