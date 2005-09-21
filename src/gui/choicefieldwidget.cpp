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

#include "choicefieldwidget.h"
#include "../field.h"

#include <kcombobox.h>

#include <qlabel.h>
#include <qlayout.h>

using Tellico::GUI::ChoiceFieldWidget;

ChoiceFieldWidget::ChoiceFieldWidget(const Data::Field* field_, QWidget* parent_, const char* name_/*=0*/)
    : FieldWidget(field_, parent_, name_), m_comboBox(0) {

  m_comboBox = new KComboBox(this);
  connect(m_comboBox, SIGNAL(activated(int)), SIGNAL(modified()));
  // always have empty choice
  m_comboBox->insertItem(QString::null);
  m_comboBox->insertStringList(field_->allowed());
  m_comboBox->setMinimumWidth(5*fontMetrics().maxWidth());

  registerWidget();
}

QString ChoiceFieldWidget::text() const {
  return m_comboBox->currentText();
}

void ChoiceFieldWidget::setText(const QString& text_) {
  blockSignals(true);

  m_comboBox->blockSignals(true);
  m_comboBox->setCurrentItem(text_);
  m_comboBox->blockSignals(false);

  blockSignals(false);
}

void ChoiceFieldWidget::clear() {
  m_comboBox->setCurrentItem(0); // first item is empty
  editMultiple(false);
}

void ChoiceFieldWidget::updateFieldHook(Data::Field*, Data::Field* newField_) {
  QString value = text();
  m_comboBox->clear();
  // always have empty choice
  m_comboBox->insertItem(QString::null);
  m_comboBox->insertStringList(newField_->allowed());
  m_comboBox->setCurrentText(value);
}

QWidget* ChoiceFieldWidget::widget() {
  return m_comboBox;
}

#include "choicefieldwidget.moc"