/***************************************************************************
    Copyright (C) 2001-2009 Robby Stephenson <robby@periapsis.org>
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

#include "derivedvalue.h"
#include "collection.h"
#include "utils/stringset.h"
#include "tellico_debug.h"

#include <QStack>

using Tellico::Data::DerivedValue;

DerivedValue::DerivedValue(const QString& valueTemplate_) : m_valueTemplate(valueTemplate_)
    , m_keyRx(QLatin1String("^([^:]+):?(-?\\d*)/?(.*)$")) {
  m_keyRx.setMinimal(true);
}

DerivedValue::DerivedValue(FieldPtr field_) : m_keyRx(QLatin1String("^([^:]+):?(-?\\d*)/?(.*)$")) {
  Q_ASSERT(field_);
  if(!field_->hasFlag(Field::Derived)) {
    myWarning() << "using DerivedValue for non-derived field";
  } else {
    m_valueTemplate = field_->property(QLatin1String("template"));
    m_fieldName = field_->name();
  }
  m_keyRx.setMinimal(true);
}

bool DerivedValue::isRecursive(Collection* coll_) const {
  Q_ASSERT(coll_);
  StringSet fieldNamesFound;
  if(!m_fieldName.isEmpty()) {
    fieldNamesFound.add(m_fieldName);
  }

  QStack<QString> fieldsToCheck;
  foreach(const QString& key, templateFields()) {
    fieldsToCheck.push(key);
  }
  while(!fieldsToCheck.isEmpty()) {
    QString fieldName = fieldsToCheck.pop();
    FieldPtr f = coll_->fieldByName(fieldName);
    if(!f) {
      f = coll_->fieldByTitle(fieldName);
    }
    if(!f) {
      continue;
    }
    if(fieldNamesFound.has(f->name())) {
      // we have recursion
      myLog() << "found recursion, refers to" << f->name() << "more than once";
      return true;
    } else {
      fieldNamesFound.add(f->name());
    }
    if(f->hasFlag(Field::Derived)) {
      DerivedValue dv(f);
      foreach(const QString& key, dv.templateFields()) {
        fieldsToCheck.push(key);
      }
    }
  }
  return false;
}

QString DerivedValue::value(EntryPtr entry_, bool formatted_) const {
  Q_ASSERT(entry_);
  Q_ASSERT(entry_->collection());
  if(!entry_ || !entry_->collection()) {
    return m_valueTemplate;
  }

  QString result;

  int endPos;
  int curPos = 0;
  int pctPos = m_valueTemplate.indexOf(QLatin1Char('%'), curPos);
  while(pctPos != -1 && pctPos+1 < m_valueTemplate.length()) {
    if(m_valueTemplate.at(pctPos+1) == QLatin1Char('{')) {
      endPos = m_valueTemplate.indexOf(QLatin1Char('}'), pctPos+2);
      if(endPos > -1) {
        result += m_valueTemplate.mid(curPos, pctPos-curPos)
                + templateKeyValue(entry_, m_valueTemplate.mid(pctPos+2, endPos-pctPos-2), formatted_);
        curPos = endPos+1;
      } else {
        break;
      }
    } else {
      result += m_valueTemplate.mid(curPos, pctPos-curPos+1);
      curPos = pctPos+1;
    }
    pctPos = m_valueTemplate.indexOf(QLatin1Char('%'), curPos);
  }
  result += m_valueTemplate.mid(curPos, m_valueTemplate.length()-curPos);
//  myDebug() << "format_ << " = " << result;
  // sometimes field value might empty, resulting in multiple consecutive white spaces
  // so let's simplify that...
  return result.simplified();
}

// format is something like "%{year} %{author}"
QStringList DerivedValue::templateFields() const {
  QRegExp rx(QLatin1String("%\\{([^:]+):?.*\\}"));
  rx.setMinimal(true);

  QStringList list;
  for(int pos = rx.indexIn(m_valueTemplate); pos > -1; pos = rx.indexIn(m_valueTemplate, pos+rx.matchedLength())) {
    list << rx.cap(1);
  }
  return list;
}

QString DerivedValue::templateKeyValue(EntryPtr entry_, const QString& key_, bool formatted_) const {
  if(m_keyRx.indexIn(key_) == -1) {
    myDebug() << "unmatched regexp for" << key_;
    return QLatin1String("%{") + key_ + QLatin1Char('}');
  }

  const QString fieldName = m_keyRx.cap(1);
  FieldPtr field = entry_->collection()->fieldByName(fieldName);
  if(!field) {
    // allow the user to also use field titles
    field = entry_->collection()->fieldByTitle(fieldName);
  }
  if(!field) {
    if(fieldName == QLatin1String("@id") ||
       fieldName == QLatin1String("id")) {
      // '@id' is the best way to use it, but formerly, we allowed just 'id'
      return QString::number(entry_->id());
    } else {
      return QLatin1String("%{") + key_ + QLatin1Char('}');
    }
  }
  // field name, followed by optional colon, optional value index (negative), and words after slash
  int pos = m_keyRx.cap(2).toInt();
  QString result;
  if(pos == 0) {
    // insert field value
    result = entry_->field(field, formatted_);
  } else {
    QStringList values = entry_->fields(field, formatted_);
    if(pos < 0) {
      pos += values.count();
      if(pos < 0) {
        pos = 0;
      }
    } else {
      // a position of 1 is actually index 0
      --pos;
    }
    // use value() instead of at() since not sure within bounds
    result = values.value(pos);
  }

  const QString func = m_keyRx.cap(3);
  if(func.contains(QLatin1Char('u'))) {
    result = result.toUpper();
  }
  if(func.contains(QLatin1Char('l'))) {
    result = result.toLower();
  }

  return result;
}