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

#include "dcimporter.h"
#include "../collections/bookcollection.h"
#include "tellico_xml.h"

using Tellico::Import::DCImporter;

DCImporter::DCImporter(const KURL& url_) : XMLImporter(url_) {
}

DCImporter::DCImporter(const QString& text_) : XMLImporter(text_) {
}

Tellico::Data::CollPtr DCImporter::collection() {
  Data::CollPtr c = new Data::BookCollection(true);

  QDomDocument doc = domDocument();
  QString dc = XML::nsDublinCore;
  QString zing = XML::nsZing;

  QRegExp authorDateRX(QString::fromLatin1(",?(\\s+\\d{4}-?(?:\\d{4})?\\.?)(.*)$"));
  QRegExp dateRX(QString::fromLatin1("\\d{4}"));

  QDomNodeList recordList = doc.elementsByTagNameNS(zing, QString::fromLatin1("recordData"));
  myDebug() << "DCImporter::collection() - number of records: " << recordList.count() << endl;

  enum { UnknownNS, UseNS, NoNS } useNS = UnknownNS;

#define GETELEMENTS(s) (useNS == NoNS) \
                         ? elem.elementsByTagName(QString::fromLatin1(s)) \
                         : elem.elementsByTagNameNS(dc, QString::fromLatin1(s))

  for(uint i = 0; i < recordList.count(); ++i) {
    Data::EntryPtr e = new Data::Entry(c);

    QDomElement elem = recordList.item(i).toElement();

    QDomNodeList nodeList = GETELEMENTS("title");
    if(nodeList.count() == 0) { // no title, skip
      if(useNS == UnknownNS) {
        nodeList = elem.elementsByTagName(QString::fromLatin1("title"));
        if(nodeList.count() > 0) {
          useNS = NoNS;
        } else {
          myDebug() << "DCImporter::collection() - no title, skipping" << endl;
          continue;
        }
      } else {
        myDebug() << "DCImporter::collection() - no title, skipping" << endl;
        continue;
      }
    } else if(useNS == UnknownNS) {
      useNS = UseNS;
    }
    QString s = nodeList.item(0).toElement().text();
    s.replace('\n', ' ');
    s = s.simplifyWhiteSpace();
    e->setField(QString::fromLatin1("title"), s);

    nodeList = GETELEMENTS("creator");
    QStringList creators;
    for(uint j = 0; j < nodeList.count(); ++j) {
      QString s = nodeList.item(j).toElement().text();
      if(authorDateRX.search(s) > -1) {
      // check if anything after date like [publisher]
        if(authorDateRX.cap(2).stripWhiteSpace().isEmpty()) {
          s.remove(authorDateRX);
          s = s.simplifyWhiteSpace();
          creators << s;
        } else {
          myDebug() << "DCImporter::collection() - wierd creator, skipping: " << s << endl;
        }
      } else {
        creators << s;
      }
    }
    e->setField(QString::fromLatin1("author"), creators.join(QString::fromLatin1("; ")));

    nodeList = GETELEMENTS("publisher");
    QStringList publishers;
    for(uint j = 0; j < nodeList.count(); ++j) {
      publishers << nodeList.item(j).toElement().text();
    }
    e->setField(QString::fromLatin1("publisher"), publishers.join(QString::fromLatin1("; ")));

    nodeList = GETELEMENTS("subject");
    QStringList keywords;
    for(uint j = 0; j < nodeList.count(); ++j) {
      keywords << nodeList.item(j).toElement().text();
    }
    e->setField(QString::fromLatin1("keyword"), keywords.join(QString::fromLatin1("; ")));

    nodeList = GETELEMENTS("date");
    if(nodeList.count() > 0) {
      QString s = nodeList.item(0).toElement().text();
      if(dateRX.search(s) > -1) {
        e->setField(QString::fromLatin1("pub_year"), dateRX.cap());
      }
    }

    nodeList = GETELEMENTS("description");
    if(nodeList.count() > 0) { // no title, skip
      e->setField(QString::fromLatin1("comments"), nodeList.item(0).toElement().text());
    }

    c->addEntries(e);
  }
#undef GETELEMENTS

  return c;
}