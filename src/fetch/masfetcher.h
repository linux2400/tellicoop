/***************************************************************************
    Copyright (C) 2012 Robby Stephenson <robby@periapsis.org>
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

#ifndef TELLICO_MASFETCHER_H
#define TELLICO_MASFETCHER_H

#include "fetcher.h"
#include "configwidget.h"
#include "../datavectors.h"

#include <QPointer>
#include <QVariantMap>

class KJob;
namespace KIO {
  class StoredTransferJob;
}

namespace QJson {
  class Parser;
}

namespace Tellico {
  namespace Fetch {

/**
 * @author Robby Stephenson
 */
class MASFetcher : public Fetcher {
Q_OBJECT

public:
  /**
   */
  MASFetcher(QObject* parent);
  /**
   */
  virtual ~MASFetcher();

  /**
   */
  virtual QString source() const;
  virtual QString attribution() const;
  virtual bool isSearching() const { return m_started; }
  virtual void continueSearch();
  virtual bool canSearch(FetchKey k) const;
  virtual void stop();
  virtual Data::EntryPtr fetchEntryHook(uint uid);
  virtual Type type() const { return MAS; }
  virtual bool canFetch(int type) const;
  virtual void readConfigHook(const KConfigGroup& config);

  /**
   * Returns a widget for modifying the fetcher's config.
   */
  virtual Fetch::ConfigWidget* configWidget(QWidget* parent) const;

  class ConfigWidget : public Fetch::ConfigWidget {
  public:
    explicit ConfigWidget(QWidget* parent_, const MASFetcher* fetcher = 0);
    virtual void saveConfigHook(KConfigGroup&);
    virtual QString preferredName() const;
  };
  friend class ConfigWidget;

  static QString defaultName();
  static QString defaultIcon();
  static StringHash allOptionalFields();

private slots:
  void slotComplete(KJob* job);

private:
  static QString value(const QVariantMap& map, const char* name, const char* name2 = 0, bool onlyFirst = false);

  virtual void search();
  virtual FetchRequest updateRequest(Data::EntryPtr entry);

  void doSearch();
  void populateEntry(Data::EntryPtr entry, const QVariantMap& result);

  QPointer<KIO::StoredTransferJob> m_job;
  QHash<int, Data::EntryPtr> m_entries;

  bool m_started;
  QJson::Parser* m_parser;
  QHash<QString, QString> m_keywordHash;

  int m_start;
  int m_total;
};

  } // end namespace
} // end namespace
#endif
