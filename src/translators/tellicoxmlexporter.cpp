/***************************************************************************
    copyright            : (C) 2003-2004 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "tellicoxmlexporter.h"
#include "../collection.h"
#include "../collections/bibtexcollection.h"
#include "../imagefactory.h"
#include "../controller.h" // needed for getting groupView pointer
#include "../groupview.h" // needed for exporting groups
#include "../tellico_utils.h" // needed for Tellico::relativeURL(...)
#include "../latin1literal.h"
#include "tellico_xml.h"

#include <klocale.h>
#include <kconfig.h>
#include <kmdcodec.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kdeversion.h>
#if KDE_IS_VERSION(3,1,90)
#include <kcalendarsystem.h>
#endif

#include <qlayout.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qwhatsthis.h>
#include <qdom.h>
#include <qtextcodec.h>

using Tellico::Export::TellicoXMLExporter;

QString TellicoXMLExporter::formatString() const {
  return i18n("XML");
}

QString TellicoXMLExporter::fileFilter() const {
  return i18n("*.xml|XML files (*.xml)") + QChar('\n') + i18n("*|All files");
}

QWidget* TellicoXMLExporter::widget(QWidget* parent_, const char* name_/*=0*/) {
  if(m_widget && m_widget->parent() == parent_) {
    return m_widget;
  }

  m_widget = new QWidget(parent_, name_);
  QVBoxLayout* l = new QVBoxLayout(m_widget);

  QGroupBox* box = new QGroupBox(1, Qt::Horizontal, i18n("Tellico XML Options"), m_widget);
  l->addWidget(box);

  m_checkIncludeImages = new QCheckBox(i18n("Include images in XML document"), box);
  m_checkIncludeImages->setChecked(m_includeImages);
  QWhatsThis::add(m_checkIncludeImages, i18n("If checked, the images in the document will be included "
                                             "in the XML stream as base64 encoded elements."));

  return m_widget;
}

void TellicoXMLExporter::readOptions(KConfig* config_) {
  KConfigGroupSaver group(config_, QString::fromLatin1("ExportOptions - %1").arg(formatString()));
  m_includeImages = config_->readBoolEntry("Include Images", m_includeImages);
}

void TellicoXMLExporter::saveOptions(KConfig* config_) {
  m_includeImages = m_checkIncludeImages->isChecked();

  KConfigGroupSaver group(config_, QString::fromLatin1("ExportOptions - %1").arg(formatString()));
  config_->writeEntry("Include Images", m_includeImages);
}

QString TellicoXMLExporter::text(bool format_, bool encodeUTF8_) {
  QDomDocument dom = exportXML(format_, encodeUTF8_);
  return dom.toString();
}

QDomDocument TellicoXMLExporter::exportXML(bool format_, bool encodeUTF8_) const {
  QDomImplementation impl;
  QDomDocumentType doctype = impl.createDocumentType(QString::fromLatin1("tellico"),
                                                     XML::pubTellico,
                                                     XML::dtdTellico);
  //default namespace
  const QString& ns = XML::nsTellico;

  QDomDocument dom = impl.createDocument(ns, QString::fromLatin1("tellico"), doctype);

  // root tellico element
  QDomElement root = dom.documentElement();

  QString encodeStr = QString::fromLatin1("version=\"1.0\" encoding=\"");
  if(encodeUTF8_) {
    encodeStr += QString::fromLatin1("UTF-8");
  } else {
    encodeStr += QString::fromLatin1(QTextCodec::codecForLocale()->mimeName());
  }
  encodeStr += QChar('"');

  // createDocument creates a root node, insert the processing instruction before it
  dom.insertBefore(dom.createProcessingInstruction(QString::fromLatin1("xml"), encodeStr), root);

  root.setAttribute(QString::fromLatin1("syntaxVersion"), XML::syntaxVersion);

  exportCollectionXML(dom, root, format_);

  // clear image list
  m_imageList.clear();

  return dom;
}

void TellicoXMLExporter::exportCollectionXML(QDomDocument& dom_, QDomElement& parent_, bool format_) const {
  QDomElement collElem = dom_.createElement(QString::fromLatin1("collection"));

  collElem.setAttribute(QString::fromLatin1("type"),      collection()->type());
  collElem.setAttribute(QString::fromLatin1("title"),     collection()->title());
  collElem.setAttribute(QString::fromLatin1("entryTitle"), collection()->entryTitle());

  QDomElement fieldsElem = dom_.createElement(QString::fromLatin1("fields"));
  collElem.appendChild(fieldsElem);
  for(Data::FieldListIterator fIt(collection()->fieldList()); fIt.current(); ++fIt) {
    exportFieldXML(dom_, fieldsElem, fIt.current());
  }

  if(collection()->type() == Data::Collection::Bibtex) {
    const Data::BibtexCollection* c = static_cast<const Data::BibtexCollection*>(collection());
    if(c) {
      if(!c->preamble().isEmpty()) {
        QDomElement preElem = dom_.createElement(QString::fromLatin1("bibtex-preamble"));
        preElem.appendChild(dom_.createTextNode(c->preamble()));
        collElem.appendChild(preElem);
      }

      QDomElement macrosElem = dom_.createElement(QString::fromLatin1("macros"));
      Data::StringMap::ConstIterator macroIt;
      for(macroIt = c->macroList().begin(); macroIt != c->macroList().end(); ++macroIt) {
        if(!macroIt.data().isEmpty()) {
          QDomElement macroElem = dom_.createElement(QString::fromLatin1("macro"));
          macroElem.setAttribute(QString::fromLatin1("name"), macroIt.key());
          macroElem.appendChild(dom_.createTextNode(macroIt.data()));
          macrosElem.appendChild(macroElem);
        }
      }
      if(macrosElem.childNodes().count() > 0) {
        collElem.appendChild(macrosElem);
      }
    }
  }

  for(Data::EntryListIterator it(entryList()); it.current(); ++it) {
    exportEntryXML(dom_, collElem, it.current(), format_);
  }

  if(!m_imageList.isEmpty()) {
    QDomElement imgsElem = dom_.createElement(QString::fromLatin1("images"));
    collElem.appendChild(imgsElem);
    for(QStringList::ConstIterator it = m_imageList.begin(); it != m_imageList.end(); ++it) {
      exportImageXML(dom_, imgsElem, ImageFactory::imageById(*it));
    }
  }

  if(m_includeGroups) {
    exportGroupXML(dom_, collElem);
  }

  parent_.appendChild(collElem);
}

void TellicoXMLExporter::exportFieldXML(QDomDocument& dom_, QDomElement& parent_, Data::Field* field_) const {
  QDomElement elem = dom_.createElement(QString::fromLatin1("field"));

  elem.setAttribute(QString::fromLatin1("name"), field_->name());
  elem.setAttribute(QString::fromLatin1("title"), field_->title());
  elem.setAttribute(QString::fromLatin1("category"), field_->category());
  elem.setAttribute(QString::fromLatin1("type"), field_->type());
  elem.setAttribute(QString::fromLatin1("flags"), field_->flags());
  elem.setAttribute(QString::fromLatin1("format"), field_->formatFlag());

  if(field_->type() == Data::Field::Choice) {
    elem.setAttribute(QString::fromLatin1("allowed"), field_->allowed().join(QString::fromLatin1(";")));
  }

  // only save description if it's not equal to title, which is the default
  // title is never empty, so this indirectly checks for empty descriptions
  if(field_->description() != field_->title()) {
    elem.setAttribute(QString::fromLatin1("description"), field_->description());
  }

  Data::StringMap::ConstIterator it;
  for(it = field_->propertyList().begin(); it != field_->propertyList().end(); ++it) {
    QDomElement e = dom_.createElement(QString::fromLatin1("prop"));
    e.setAttribute(QString::fromLatin1("name"), it.key());
    e.appendChild(dom_.createTextNode(it.data()));
    elem.appendChild(e);
  }

  parent_.appendChild(elem);
}

void TellicoXMLExporter::exportEntryXML(QDomDocument& dom_, QDomElement& parent_, Data::Entry* entry_, bool format_) const {
  QDomElement entryElem = dom_.createElement(QString::fromLatin1("entry"));

  entryElem.setAttribute(QString::fromLatin1("id"), entry_->id());

  // iterate through every field for the entry
  for(Data::FieldListIterator fIt(entry_->collection()->fieldList()); fIt.current(); ++fIt) {
    QString fieldName = fIt.current()->name();

    // Date fields are special, don't format in export
    QString fieldValue = (format_ && fIt.current()->type() != Data::Field::Date) ? entry_->formattedField(fieldName) : entry_->field(fieldName);

    // if empty, then no field element is added and just continue
    if(fieldValue.isEmpty()) {
      continue;
    }

    // if multiple versions are allowed, split them into separate elements
    if(fIt.current()->flags() & Data::Field::AllowMultiple) {
      // parent element if field contains multiple values, child of entryElem
      // who cares about grammar, just add an 's' to the name
      QDomElement parElem = dom_.createElement(fieldName + QString::fromLatin1("s"));
      entryElem.appendChild(parElem);

      // the space after the semi-colon is enforced when the field is set for the entry
      QStringList fields = QStringList::split(QString::fromLatin1("; "), fieldValue, true);
      for(QStringList::ConstIterator it = fields.begin(); it != fields.end(); ++it) {
        // element for field value, child of either entryElem or ParentElem
        QDomElement fieldElem = dom_.createElement(fieldName);
        // special case for 2-column tables
        if(fIt.current()->type() == Data::Field::Table2) {
          QDomElement elem1, elem2;
          elem1 = dom_.createElement(QString::fromLatin1("column"));
          elem2 = dom_.createElement(QString::fromLatin1("column"));
          elem1.appendChild(dom_.createTextNode((*it).section(QString::fromLatin1("::"), 0, 0)));
          elem2.appendChild(dom_.createTextNode((*it).section(QString::fromLatin1("::"), 1)));
          fieldElem.appendChild(elem1);
          fieldElem.appendChild(elem2);
        } else {
          fieldElem.appendChild(dom_.createTextNode(*it));
        }
        parElem.appendChild(fieldElem);
      }
    } else {
      QDomElement fieldElem = dom_.createElement(fieldName);
      entryElem.appendChild(fieldElem);
      // Date fields get special treatment
      if(fIt.current()->type() == Data::Field::Date) {
#if KDE_IS_VERSION(3,1,90)
        fieldElem.setAttribute(QString::fromLatin1("calendar"), KGlobal::locale()->calendar()->calendarName());
#else
        fieldElem.setAttribute(QString::fromLatin1("calendar"), QString::fromLatin1("gregorian"));
#endif
        QStringList s = QStringList::split('-', fieldValue, true);
        if(s.count() > 0 && !s[0].isEmpty()) {
          QDomElement e = dom_.createElement(QString::fromLatin1("year"));
          fieldElem.appendChild(e);
          e.appendChild(dom_.createTextNode(s[0]));
        }
        if(s.count() > 1 && !s[1].isEmpty()) {
          QDomElement e = dom_.createElement(QString::fromLatin1("month"));
          fieldElem.appendChild(e);
          e.appendChild(dom_.createTextNode(s[1]));
        }
        if(s.count() > 2 && !s[2].isEmpty()) {
          QDomElement e = dom_.createElement(QString::fromLatin1("day"));
          fieldElem.appendChild(e);
          e.appendChild(dom_.createTextNode(s[2]));
        }
// relative URLs are not ready yet
#if 0
      // special case for saving relative URLs
      } else if(fIt.current()->type() == Data::Field::URL
                && fIt.current()->property(QString::fromLatin1("relative")) == Latin1Literal("true")) {
        QString s = fieldValue;
        if(!KURL::isRelativeURL(s)) {
#if KDE_IS_VERSION(3,1,90)
          s = KURL::relativeURL(url(), KURL(fieldValue));
#else
          s = Tellico::relativeURL(url(), KURL(fieldValue));
#endif
        }
        fieldElem.appendChild(dom_.createTextNode(s));
#endif
      } else {
        fieldElem.appendChild(dom_.createTextNode(fieldValue));
      }
    }

    if(fIt.current()->type() == Data::Field::Image) {
      // possible to have more than one entry with the same image
      // only want to include it in the output xml once
      if(m_imageList.findIndex(fieldValue) == -1) {
        m_imageList += fieldValue;
      }
    }
  } // end field loop

  parent_.appendChild(entryElem);
}

void TellicoXMLExporter::exportImageXML(QDomDocument& dom_, QDomElement& parent_, const Data::Image& img_) const {
  if(img_.isNull()) {
    kdDebug() << "TellicoXMLExporter::exportImageXML() - null image!" << endl;
    return;
  }
//  kdDebug() << "TellicoXMLExporter::exportImageXML() - id = " << img_.id() << endl;

  QDomElement imgElem = dom_.createElement(QString::fromLatin1("image"));
  imgElem.setAttribute(QString::fromLatin1("format"), img_.format());
  imgElem.setAttribute(QString::fromLatin1("id"), img_.id());
  imgElem.setAttribute(QString::fromLatin1("width"), img_.width());
  imgElem.setAttribute(QString::fromLatin1("height"), img_.height());
  if(m_includeImages) {
    QCString imgText = KCodecs::base64Encode(img_.byteArray());
    imgElem.appendChild(dom_.createTextNode(QString::fromLatin1(imgText)));
  }
  parent_.appendChild(imgElem);
}

void TellicoXMLExporter::exportGroupXML(QDomDocument& dom_, QDomElement& parent_) const {
  const GroupView* const gv = Controller::self()->groupView();
  // iterator over each group, which are the children of the root item
  QListViewItem* rItem = gv->firstChild();
  if(!rItem) {
    return;
  }
  for(QListViewItem* gItem = rItem->firstChild(); gItem; gItem = gItem->nextSibling()) {
    QDomElement groupElem = dom_.createElement(QString::fromLatin1("group"));
    groupElem.setAttribute(QString::fromLatin1("title"), gItem->text(0));
    // now iterate over all entry items in the group
    for(QListViewItem* eItem = gItem->firstChild(); eItem; eItem = eItem->nextSibling()) {
      QDomElement entryRefElem = dom_.createElement(QString::fromLatin1("entryRef"));
      entryRefElem.setAttribute(QString::fromLatin1("id"), static_cast<EntryItem*>(eItem)->entry()->id());
      groupElem.appendChild(entryRefElem);
    }
    parent_.appendChild(groupElem);
  }
}