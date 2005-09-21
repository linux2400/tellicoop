/***************************************************************************
    copyright            : (C) 2005 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "ratingfieldwidget.h"
#include "ratingwidget.h"
#include "../field.h"

using Tellico::GUI::RatingFieldWidget;

RatingFieldWidget::RatingFieldWidget(const Data::Field* field_, QWidget* parent_, const char* name_/*=0*/)
    : FieldWidget(field_, parent_, name_) {
  m_rating = new RatingWidget(field_, this);
  connect(m_rating, SIGNAL(modified()), SIGNAL(modified()));

  registerWidget();
}

QString RatingFieldWidget::text() const {
  return m_rating->text();
}

void RatingFieldWidget::setText(const QString& text_) {
  blockSignals(true);

  m_rating->blockSignals(true);
  m_rating->setText(text_);
  m_rating->blockSignals(false);

  blockSignals(false);

  if(m_rating->text() != text_) {
    emit modified();
  }
}

void RatingFieldWidget::clear() {
  m_rating->clear();
  editMultiple(false);
}

void RatingFieldWidget::updateFieldHook(Data::Field*, Data::Field* newField_) {
  m_rating->updateField(newField_);
}

QWidget* RatingFieldWidget::widget() {
  return m_rating;
}

#include "ratingfieldwidget.moc"