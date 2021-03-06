/***************************************************************************
    Copyright (C) 2010-2011 Robby Stephenson <robby@periapsis.org>
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

#include "gcstarfetchertest.h"
#include "gcstarfetchertest.moc"
#include "qtest_kde.h"

#include "../fetch/gcstarpluginfetcher.h"
#include "../entry.h"
#include "../collections/videocollection.h"
#include "../collectionfactory.h"
#include "../images/imagefactory.h"
#include "../images/image.h"
#include "../fieldformat.h"

#include <KConfigGroup>
#include <KStandardDirs>

QTEST_KDEMAIN( GCstarFetcherTest, GUI )

GCstarFetcherTest::GCstarFetcherTest() : AbstractFetcherTest() {
}

void GCstarFetcherTest::initTestCase() {
  const QString gcstar = KStandardDirs::findExe(QLatin1String("gcstar"));
  if(gcstar.isEmpty()) {
    QSKIP("This test requires gcstar", SkipAll);
  }

  Tellico::ImageFactory::init();
  Tellico::RegisterCollection<Tellico::Data::VideoCollection> registerVideo(Tellico::Data::Collection::Video, "video");

  KGlobal::dirs()->addResourceDir("appdata", QString::fromLatin1(KDESRCDIR) + "/../../xslt/");
}

void GCstarFetcherTest::testSnowyRiver() {
  KConfig config(QString::fromLatin1(KDESRCDIR)  + "/tellicotest.config", KConfig::SimpleConfig);
  QString groupName = QLatin1String("GCstar Video");
  if(!config.hasGroup(groupName)) {
    QSKIP("This test requires a config file.", SkipAll);
  }
  KConfigGroup cg(&config, groupName);

  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Video, Tellico::Fetch::Title, "Superman Returns");
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::GCstarPluginFetcher(this));
  fetcher->readConfig(cg, cg.name());

  Tellico::Data::EntryList results = DO_FETCH1(fetcher, request, 1);

  QCOMPARE(results.size(), 1);

  // the first entry had better be the right one
  Tellico::Data::EntryPtr entry = results.at(0);

  QCOMPARE(entry->field("title"), QLatin1String("Superman Returns"));
  QCOMPARE(entry->field("year"), QLatin1String("2006"));
  QVERIFY(!entry->field("cover").isEmpty());
  QVERIFY(!Tellico::ImageFactory::imageById(entry->field("cover")).isNull());
}
