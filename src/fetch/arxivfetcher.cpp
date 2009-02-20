/***************************************************************************
    copyright            : (C) 2007-2008 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "arxivfetcher.h"
#include "messagehandler.h"
#include "../translators/xslthandler.h"
#include "../translators/tellicoimporter.h"
#include "../tellico_kernel.h"
#include "../tellico_utils.h"
#include "../collection.h"
#include "../entry.h"
#include "../core/netaccess.h"
#include "../imagefactory.h"
#include "../tellico_debug.h"

#include <klocale.h>
#include <kio/job.h>
#include <kio/jobuidelegate.h>
#include <kstandarddirs.h>
#include <KConfigGroup>

#include <QDomDocument>
#include <QLabel>
#include <QTextStream>
#include <QPixmap>
#include <QVBoxLayout>

//#define ARXIV_TEST

namespace {
  static const int ARXIV_RETURNS_PER_REQUEST = 20;
  static const char* ARXIV_BASE_URL = "http://export.arxiv.org/api/query";
}

using Tellico::Fetch::ArxivFetcher;

ArxivFetcher::ArxivFetcher(QObject* parent_)
    : Fetcher(parent_), m_xsltHandler(0), m_start(0), m_job(0), m_started(false) {
}

ArxivFetcher::~ArxivFetcher() {
  delete m_xsltHandler;
  m_xsltHandler = 0;
}

QString ArxivFetcher::defaultName() {
  return i18n("arXiv.org");
}

QString ArxivFetcher::source() const {
  return m_name.isEmpty() ? defaultName() : m_name;
}

bool ArxivFetcher::canFetch(int type) const {
  return type == Data::Collection::Bibtex;
}

void ArxivFetcher::readConfigHook(const KConfigGroup&) {
}

void ArxivFetcher::search(Tellico::Fetch::FetchKey key_, const QString& value_) {
  m_key = key_;
  m_value = value_.trimmed();
  m_started = true;
  m_start = 0;
  m_total = -1;
  doSearch();
}

void ArxivFetcher::continueSearch() {
  m_started = true;
  doSearch();
}

void ArxivFetcher::doSearch() {
  if(!canFetch(Kernel::self()->collectionType())) {
    message(i18n("%1 does not allow searching for this collection type.", source()), MessageHandler::Warning);
    stop();
    return;
  }

//  myDebug() << "ArxivFetcher::search() - value = " << value_ << endl;

  KUrl u = searchURL(m_key, m_value);
  if(u.isEmpty()) {
    stop();
    return;
  }

  m_job = KIO::storedGet(u, KIO::NoReload, KIO::HideProgressInfo);
  m_job->ui()->setWindow(Kernel::self()->widget());
  connect(m_job, SIGNAL(result(KJob*)),
          SLOT(slotComplete(KJob*)));
}

void ArxivFetcher::stop() {
  if(!m_started) {
    return;
  }
//  myDebug() << "ArxivFetcher::stop()" << endl;
  if(m_job) {
    m_job->kill();
    m_job = 0;
  }
  m_started = false;
  emit signalDone(this);
}

void ArxivFetcher::slotComplete(KJob*) {
//  myDebug() << "ArxivFetcher::slotComplete()" << endl;

  if(m_job->error()) {
    m_job->ui()->showErrorMessage();
    stop();
    return;
  }

  QByteArray data = m_job->data();
  if(data.isEmpty()) {
    myDebug() << "ArxivFetcher::slotComplete() - no data" << endl;
    stop();
    return;
  }

  // since the fetch is done, don't worry about holding the job pointer
  m_job = 0;
#if 0
  kWarning() << "Remove debug from arxivfetcher.cpp";
  QFile f(QLatin1String("/tmp/test.xml"));
  if(f.open(QIODevice::WriteOnly)) {
    QTextStream t(&f);
    t.setEncoding(QTextStream::UnicodeUTF8);
    t << data;
  }
  f.close();
#endif

  if(!m_xsltHandler) {
    initXSLTHandler();
    if(!m_xsltHandler) { // probably an error somewhere in the stylesheet loading
      stop();
      return;
    }
  }

  if(m_total == -1) {
    QDomDocument dom;
    if(!dom.setContent(data, true /*namespace*/)) {
      kWarning() << "ArxivFetcher::slotComplete() - server did not return valid XML.";
      return;
    }
    // total is top level element, with attribute totalResultsAvailable
    QDomNodeList list = dom.elementsByTagNameNS(QLatin1String("http://a9.com/-/spec/opensearch/1.1/"),
                                                QLatin1String("totalResults"));
    if(list.count() > 0) {
      m_total = list.item(0).toElement().text().toInt();
    }
  }

  // assume result is always utf-8
  QString str = m_xsltHandler->applyStylesheet(QString::fromUtf8(data, data.size()));
  Import::TellicoImporter imp(str);
  Data::CollPtr coll = imp.collection();

  if(!coll) {
    myDebug() << "ArxivFetcher::slotComplete() - no valid result" << endl;
    stop();
    return;
  }

  Data::EntryList entries = coll->entries();
  foreach(Data::EntryPtr entry, entries) {
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
  }

  m_start = m_entries.count();
  m_hasMoreResults = m_start < m_total;
  stop(); // required
}

Tellico::Data::EntryPtr ArxivFetcher::fetchEntry(uint uid_) {
  Data::EntryPtr entry = m_entries[uid_];
  // if URL but no cover image, fetch it
  if(!entry->field(QLatin1String("url")).isEmpty()) {
    Data::CollPtr coll = entry->collection();
    Data::FieldPtr field = coll->fieldByName(QLatin1String("cover"));
    if(!field && !coll->imageFields().isEmpty()) {
      field = coll->imageFields().front();
    } else if(!field) {
      field = new Data::Field(QLatin1String("cover"), i18n("Front Cover"), Data::Field::Image);
      coll->addField(field);
    }
    if(entry->field(field).isEmpty()) {
      QPixmap pix = NetAccess::filePreview(entry->field(QLatin1String("url")));
      if(!pix.isNull()) {
        QString id = ImageFactory::addImage(pix, QLatin1String("PNG"));
        if(!id.isEmpty()) {
          entry->setField(field, id);
        }
      }
    }
  }
  QRegExp versionRx(QLatin1String("v\\d+$"));
  // if the original search was not for a versioned ID, remove it
  if(m_key != ArxivID || !m_value.contains(versionRx)) {
    QString arxiv = entry->field(QLatin1String("arxiv"));
    arxiv.remove(versionRx);
    entry->setField(QLatin1String("arxiv"), arxiv);
  }
  return entry;
}

void ArxivFetcher::initXSLTHandler() {
  QString xsltfile = KStandardDirs::locate("appdata", QLatin1String("arxiv2tellico.xsl"));
  if(xsltfile.isEmpty()) {
    kWarning() << "ArxivFetcher::initXSLTHandler() - can not locate arxiv2tellico.xsl.";
    return;
  }

  KUrl u;
  u.setPath(xsltfile);

  delete m_xsltHandler;
  m_xsltHandler = new XSLTHandler(u);
  if(!m_xsltHandler->isValid()) {
    kWarning() << "ArxivFetcher::initXSLTHandler() - error in arxiv2tellico.xsl.";
    delete m_xsltHandler;
    m_xsltHandler = 0;
    return;
  }
}

KUrl ArxivFetcher::searchURL(Tellico::Fetch::FetchKey key_, const QString& value_) const {
  KUrl u(ARXIV_BASE_URL);
  u.addQueryItem(QLatin1String("start"), QString::number(m_start));
  u.addQueryItem(QLatin1String("max_results"), QString::number(ARXIV_RETURNS_PER_REQUEST));

  // quotes should be used if spaces are present, just use all the time
  QString quotedValue = '"' + value_ + '"';
  switch(key_) {
    case Title:
      u.addQueryItem(QLatin1String("search_query"), QString::fromLatin1("ti:%1").arg(quotedValue));
      break;

    case Person:
      u.addQueryItem(QLatin1String("search_query"), QString::fromLatin1("au:%1").arg(quotedValue));
      break;

    case Keyword:
      // keyword gets to use all the words without being quoted
      u.addQueryItem(QLatin1String("search_query"), QString::fromLatin1("all:%1").arg(value_));
      break;

    case ArxivID:
      {
      // remove prefix and/or version number
      QString value = value_;
      value.remove(QRegExp(QLatin1String("^arxiv:"), Qt::CaseInsensitive));
      value.remove(QRegExp(QLatin1String("v\\d+$")));
      u.addQueryItem(QLatin1String("search_query"), QString::fromLatin1("id:%1").arg(value));
      }
      break;

    default:
      kWarning() << "ArxivFetcher::search() - key not recognized: " << m_key;
      return KUrl();
  }

#ifdef ARXIV_TEST
  u = KUrl::fromPathOrUrl("/home/robby/arxiv.xml");
#endif
  myDebug() << "ArxivFetcher::search() - url: " << u.url() << endl;
  return u;
}

void ArxivFetcher::updateEntry(Tellico::Data::EntryPtr entry_) {
  QString id = entry_->field(QLatin1String("arxiv"));
  if(!id.isEmpty()) {
    search(Fetch::ArxivID, id);
    return;
  }

  // optimistically try searching for title and rely on Collection::sameEntry() to figure things out
  QString t = entry_->field(QLatin1String("title"));
  if(!t.isEmpty()) {
    search(Fetch::Title, t);
    return;
  }

  myDebug() << "ArxivFetcher::updateEntry() - insufficient info to search" << endl;
  emit signalDone(this); // always need to emit this if not continuing with the search
}

void ArxivFetcher::updateEntrySynchronous(Tellico::Data::EntryPtr entry) {
  if(!entry) {
    return;
  }
  QString arxiv = entry->field(QLatin1String("arxiv"));
  if(arxiv.isEmpty()) {
    return;
  }

  KUrl u = searchURL(ArxivID, arxiv);
  QString xml = FileHandler::readTextFile(u, true, true);
  if(xml.isEmpty()) {
    return;
  }

  if(!m_xsltHandler) {
    initXSLTHandler();
    if(!m_xsltHandler) { // probably an error somewhere in the stylesheet loading
      return;
    }
  }

  // assume result is always utf-8
  QString str = m_xsltHandler->applyStylesheet(xml);
  Import::TellicoImporter imp(str);
  Data::CollPtr coll = imp.collection();
  if(coll && coll->entryCount() > 0) {
    myLog() << "ArxivFetcher::updateEntrySynchronous() - found Arxiv result, merging" << endl;
    Data::Collection::mergeEntry(entry, coll->entries().front(), false /*overwrite*/);
    // the arxiv id might have a version#
    entry->setField(QLatin1String("arxiv"),
                    coll->entries().front()->field(QLatin1String("arxiv")));
  }
}

Tellico::Fetch::ConfigWidget* ArxivFetcher::configWidget(QWidget* parent_) const {
  return new ArxivFetcher::ConfigWidget(parent_, this);
}

ArxivFetcher::ConfigWidget::ConfigWidget(QWidget* parent_, const ArxivFetcher*)
    : Fetch::ConfigWidget(parent_) {
  QVBoxLayout* l = new QVBoxLayout(optionsWidget());
  l->addWidget(new QLabel(i18n("This source has no options."), optionsWidget()));
  l->addStretch();
}

void ArxivFetcher::ConfigWidget::saveConfig(KConfigGroup&) {
}

QString ArxivFetcher::ConfigWidget::preferredName() const {
  return ArxivFetcher::defaultName();
}

#include "arxivfetcher.moc"