/***************************************************************************
    copyright            : (C) 2006 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifndef TELLICO_FETCH_ISBNDBFETCHER_H
#define TELLICO_FETCH_ISBNDBFETCHER_H

namespace Tellico {
  class XSLTHandler;
}

#include "fetcher.h"
#include "configwidget.h"
#include "../datavectors.h"

#include <kurl.h>
#include <kio/job.h>

#include <qcstring.h> // for QByteArray
#include <qguardedptr.h>

namespace Tellico {
  namespace Fetch {

/**
 * @author Robby Stephenson
 */
class ISBNdbFetcher : public Fetcher {
Q_OBJECT

public:
  ISBNdbFetcher(QObject* parent = 0, const char* name = 0);
  ~ISBNdbFetcher();

  virtual QString source() const;
  virtual bool isSearching() const { return m_started; }
  virtual void search(FetchKey key, const QString& value);
  virtual bool canSearch(FetchKey k) const { return k == Title || k == ISBN; }
  virtual void stop();
  virtual Data::EntryPtr fetchEntry(uint uid);
  virtual Type type() const { return ISBNdb; }
  virtual bool canFetch(int type) const;
  virtual void readConfigHook(KConfig* config, const QString& group);

  virtual void updateEntry(Data::EntryPtr entry);

  virtual Fetch::ConfigWidget* configWidget(QWidget* parent) const;

  class ConfigWidget : public Fetch::ConfigWidget {
  public:
    ConfigWidget(QWidget* parent_, const ISBNdbFetcher* fetcher = 0);
    virtual void saveConfig(KConfig*) {}
    virtual QString preferredName() const;
  };
  friend class ConfigWidget;

  static QString defaultName();

private slots:
  void slotData(KIO::Job* job, const QByteArray& data);
  void slotComplete(KIO::Job* job);

private:
  void initXSLTHandler();

  XSLTHandler* m_xsltHandler;
  int m_limit;

  QByteArray m_data;
  QMap<int, Data::EntryPtr> m_entries;
  QGuardedPtr<KIO::Job> m_job;

  bool m_started;
};

  }
}
#endif