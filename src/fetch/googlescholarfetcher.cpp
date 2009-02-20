/***************************************************************************
    copyright            : (C) 2008 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "googlescholarfetcher.h"
#include "messagehandler.h"
#include "../filehandler.h"
#include "../translators/bibteximporter.h"
#include "../collection.h"
#include "../entry.h"
#include "../tellico_kernel.h"
#include "../tellico_debug.h"

#include <klocale.h>
#include <KConfigGroup>
#include <kio/job.h>
#include <kio/jobuidelegate.h>

#include <QLabel>
#include <QVBoxLayout>

namespace {
  static const int GOOGLE_MAX_RETURNS_TOTAL = 20;
  static const char* SCHOLAR_BASE_URL = "http://scholar.google.com/scholar";
  static const char* SCHOLAR_SET_BIBTEX_URL = "http://scholar.google.com/scholar_setprefs?num=100&scis=yes&scisf=4&submit=Save+Preferences";
}

using Tellico::Fetch::GoogleScholarFetcher;

GoogleScholarFetcher::GoogleScholarFetcher(QObject* parent_)
    : Fetcher(parent_),
      m_limit(GOOGLE_MAX_RETURNS_TOTAL), m_start(0), m_job(0), m_started(false),
      m_cookieIsSet(false) {
  m_bibtexRx = QRegExp(QLatin1String("<a\\s.*href\\s*=\\s*\"([^>]*scholar\\.bib[^>]*)\""));
  m_bibtexRx.setMinimal(true);
}

GoogleScholarFetcher::~GoogleScholarFetcher() {
}

QString GoogleScholarFetcher::defaultName() {
  // no i18n
  return QLatin1String("Google Scholar");
}

QString GoogleScholarFetcher::source() const {
  return m_name.isEmpty() ? defaultName() : m_name;
}

bool GoogleScholarFetcher::canFetch(int type) const {
  return type == Data::Collection::Bibtex;
}

void GoogleScholarFetcher::readConfigHook(const KConfigGroup& config_) {
  Q_UNUSED(config_);
}

void GoogleScholarFetcher::search(Tellico::Fetch::FetchKey key_, const QString& value_) {
  if(!m_cookieIsSet) {
    // have to set preferences to have bibtex output
    FileHandler::readTextFile(KUrl(SCHOLAR_SET_BIBTEX_URL), true);
    m_cookieIsSet = true;
  }
  m_key = key_;
  m_value = value_;
  m_started = true;
  m_start = 0;
  m_total = -1;
  doSearch();
}

void GoogleScholarFetcher::continueSearch() {
  m_started = true;
  doSearch();
}

void GoogleScholarFetcher::doSearch() {
//  myDebug() << "GoogleScholarFetcher::search() - value = " << value_ << endl;

  if(!canFetch(Kernel::self()->collectionType())) {
    message(i18n("%1 does not allow searching for this collection type.", source()), MessageHandler::Warning);
    stop();
    return;
  }

  KUrl u(SCHOLAR_BASE_URL);
  u.addQueryItem(QLatin1String("start"), QString::number(m_start));

  switch(m_key) {
    case Title:
      u.addQueryItem(QLatin1String("q"), QString::fromLatin1("allintitle:%1").arg(m_value));
      break;

    case Keyword:
      u.addQueryItem(QLatin1String("q"), m_value);
      break;

    case Person:
      u.addQueryItem(QLatin1String("q"), QString::fromLatin1("author:%1").arg(m_value));
      break;

    default:
      kWarning() << "GoogleScholarFetcher::search() - key not recognized: " << m_key;
      stop();
      return;
  }
//  myDebug() << "GoogleScholarFetcher::search() - url: " << u.url() << endl;

  m_job = KIO::storedGet(u, KIO::NoReload, KIO::HideProgressInfo);
  m_job->ui()->setWindow(Kernel::self()->widget());
  connect(m_job, SIGNAL(result(KJob*)),
          SLOT(slotComplete(KJob*)));
}

void GoogleScholarFetcher::stop() {
  if(!m_started) {
    return;
  }
  if(m_job) {
    m_job->kill();
    m_job = 0;
  }
  m_started = false;
  emit signalDone(this);
}

void GoogleScholarFetcher::slotComplete(KJob*) {
//  myDebug() << "GoogleScholarFetcher::slotComplete()" << endl;

  if(m_job->error()) {
    m_job->ui()->showErrorMessage();
    stop();
    return;
  }

  QByteArray data = m_job->data();
  if(data.isEmpty()) {
    myDebug() << "GoogleScholarFetcher::slotComplete() - no data" << endl;
    stop();
    return;
  }

  // since the fetch is done, don't worry about holding the job pointer
  m_job = 0;

  QString text = QString::fromUtf8(data, data.size());
  QString bibtex;
  int count = 0;
  for(int pos = m_bibtexRx.indexIn(text); count < m_limit && pos > -1; pos = m_bibtexRx.indexIn(text, pos+m_bibtexRx.matchedLength()), ++count) {
    KUrl bibtexUrl(KUrl(SCHOLAR_BASE_URL), m_bibtexRx.cap(1));
//    myDebug() << bibtexUrl << endl;
    bibtex += FileHandler::readTextFile(bibtexUrl, true);
  }

  Import::BibtexImporter imp(bibtex);
  Data::CollPtr coll = imp.collection();
  if(!coll) {
    myDebug() << "GoogleScholarFetcher::slotComplete() - no collection pointer" << endl;
    stop();
    return;
  }

  count = 0;
  Data::EntryList entries = coll->entries();
  foreach(Data::EntryPtr entry, entries) {
    if(count >= m_limit) {
      break;
    }
    if(!m_started) {
      // might get aborted
      break;
    }
    QString desc = entry->field(QLatin1String("author"))
                 + QChar('/') + entry->field(QLatin1String("publisher"));
    if(!entry->field(QLatin1String("year")).isEmpty()) {
      desc += QChar('/') + entry->field(QLatin1String("year"));
    }

    SearchResult* r = new SearchResult(Fetcher::Ptr(this), entry->title(), desc, entry->field(QLatin1String("isbn")));
    m_entries.insert(r->uid, Data::EntryPtr(entry));
    emit signalResultFound(r);
    ++count;
  }
  m_start = m_entries.count();
//  m_hasMoreResults = m_start <= m_total;
  m_hasMoreResults = false; // for now, no continued searches

  stop(); // required
}

Tellico::Data::EntryPtr GoogleScholarFetcher::fetchEntry(uint uid_) {
  return m_entries[uid_];
}

void GoogleScholarFetcher::updateEntry(Tellico::Data::EntryPtr entry_) {
//  myDebug() << "GoogleScholarFetcher::updateEntry()" << endl;
  // limit to top 5 results
  m_limit = 5;

  QString title = entry_->field(QLatin1String("title"));
  if(!title.isEmpty()) {
    search(Title, title);
    return;
  }

  myDebug() << "GoogleScholarFetcher::updateEntry() - insufficient info to search" << endl;
  emit signalDone(this); // always need to emit this if not continuing with the search
}

Tellico::Fetch::ConfigWidget* GoogleScholarFetcher::configWidget(QWidget* parent_) const {
  return new GoogleScholarFetcher::ConfigWidget(parent_, this);
}

GoogleScholarFetcher::ConfigWidget::ConfigWidget(QWidget* parent_, const GoogleScholarFetcher* /*=0*/)
    : Fetch::ConfigWidget(parent_) {
  QVBoxLayout* l = new QVBoxLayout(optionsWidget());
  l->addWidget(new QLabel(i18n("This source has no options."), optionsWidget()));
  l->addStretch();
}

QString GoogleScholarFetcher::ConfigWidget::preferredName() const {
  return GoogleScholarFetcher::defaultName();
}

#include "googlescholarfetcher.moc"