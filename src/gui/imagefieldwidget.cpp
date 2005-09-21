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

#include "imagefieldwidget.h"
#include "imagewidget.h"
#include "../field.h"

using Tellico::GUI::ImageFieldWidget;

ImageFieldWidget::ImageFieldWidget(const Data::Field* field_, QWidget* parent_, const char* name_/*=0*/)
    : FieldWidget(field_, parent_, name_) {

  m_widget = new ImageWidget(this);
  connect(m_widget, SIGNAL(signalModified()), SIGNAL(modified()));

  registerWidget();
}

QString ImageFieldWidget::text() const {
  return m_widget->id();
}

void ImageFieldWidget::setText(const QString& text_) {
  blockSignals(true);
  m_widget->blockSignals(true);

  m_widget->setImage(text_);

  m_widget->blockSignals(false);
  blockSignals(false);
}

void ImageFieldWidget::clear() {
  m_widget->slotClear();
  editMultiple(false);
}

QWidget* ImageFieldWidget::widget() {
  return m_widget;
}

#include "imagefieldwidget.moc"