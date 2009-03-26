/***************************************************************************
    copyright            : (C) 2007-2009 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "fieldcomparison.h"
#include "stringcomparison.h"
#include "../field.h"
#include "../collection.h"
#include "../document.h"
#include "../imagefactory.h"
#include "../image.h"

#include <QPixmap>
#include <QDateTime>

Tellico::FieldComparison* Tellico::FieldComparison::create(Data::FieldPtr field_) {
  if(field_->type() == Data::Field::Number) {
    return new ValueComparison(field_, new NumberComparison());
  } else if(field_->type() == Data::Field::Bool) {
    return new ValueComparison(field_, new BoolComparison());
  } else if(field_->type() == Data::Field::Rating) {
    return new ValueComparison(field_, new RatingComparison());
  } else if(field_->type() == Data::Field::Image) {
    return new ImageComparison(field_);
  } else if(field_->type() == Data::Field::Dependent) {
    return new DependentComparison(field_);
  } else if(field_->type() == Data::Field::Date || field_->formatFlag() == Data::Field::FormatDate) {
    return new ValueComparison(field_, new ISODateComparison());
  } else if(field_->type() == Data::Field::Choice) {
    return new ChoiceComparison(field_);
  } else if(field_->formatFlag() == Data::Field::FormatTitle) {
    // Dependent could be title, so put this test after
    return new ValueComparison(field_, new TitleComparison());
  } else if(field_->property(QLatin1String("lcc")) == QLatin1String("true") ||
            field_->name() == QLatin1String("lcc")) {
    // allow LCC comparison if LCC property is set, or if the name is lcc
    return new ValueComparison(field_, new LCCComparison());
  }
  return new ValueComparison(field_, new StringComparison());
}

Tellico::FieldComparison::FieldComparison(Data::FieldPtr field_) : m_field(field_) {
}

int Tellico::FieldComparison::compare(Data::EntryPtr entry1_, Data::EntryPtr entry2_) {
  return compare(entry1_->field(m_field), entry2_->field(m_field));
}

Tellico::ValueComparison::ValueComparison(Data::FieldPtr field, StringComparison* comp)
    : FieldComparison(field)
    , m_stringComparison(comp) {
  Q_ASSERT(comp);
}

Tellico::ValueComparison::~ValueComparison() {
  delete m_stringComparison;
}

int Tellico::ValueComparison::compare(const QString& str1_, const QString& str2_) {
  return m_stringComparison->compare(str1_, str2_);
}

Tellico::ImageComparison::ImageComparison(Data::FieldPtr field) : FieldComparison(field) {
}

int Tellico::ImageComparison::compare(const QString& str1_, const QString& str2_) {
  if(str1_.isEmpty()) {
    if(str2_.isEmpty()) {
      return 0;
    }
    return -1;
  }
  if(str2_.isEmpty()) {
    return 1;
  }

  const Data::Image& image1 = ImageFactory::imageById(str1_);
  const Data::Image& image2 = ImageFactory::imageById(str2_);
  if(image1.isNull()) {
    if(image2.isNull()) {
      return 0;
    }
    return -1;
  }
  if(image2.isNull()) {
    return 1;
  }
  // large images come first
  return image1.width() - image2.width();
}

Tellico::DependentComparison::DependentComparison(Data::FieldPtr field) : FieldComparison(field) {
  Data::FieldList fields = Data::Document::self()->collection()->fieldDependsOn(field);
  foreach(Data::FieldPtr field, fields) {
    m_comparisons.append(create(field));
  }
}

Tellico::DependentComparison::~DependentComparison() {
  qDeleteAll(m_comparisons);
  m_comparisons.clear();
}

int Tellico::DependentComparison::compare(Data::EntryPtr entry1_, Data::EntryPtr entry2_) {
  foreach(FieldComparison* comp, m_comparisons) {
    int res = comp->compare(entry1_, entry2_);
    if(res != 0) {
      return res;
    }
  }
  return FieldComparison::compare(entry1_, entry2_);
}

Tellico::ChoiceComparison::ChoiceComparison(Data::FieldPtr field) : FieldComparison(field) {
  m_values = field->allowed();
}

int Tellico::ChoiceComparison::compare(const QString& str1, const QString& str2) {
  return m_values.indexOf(str1) - m_values.indexOf(str2);
}