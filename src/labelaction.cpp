/***************************************************************************
    copyright            : (C) 2002-2004 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "labelaction.h"

#include <ktoolbar.h>
#include <klineedit.h>

using Bookcase::LabelAction;
using Bookcase::LineEditAction;

#if KDE_VERSION > 309
#include <qlabel.h>
// KWidgetAction(QWidget *widget,
//               const QString &text,
//               const KShortcut &cut,
//               const QObject *receiver,
//               const char *slot,
//               KActionCollection *parent
//               const char *name)
LabelAction::LabelAction(const QString& text_, int accel_,
                         KActionCollection* parent_, const char* name_/*=0*/)
    : KWidgetAction(new QLabel(text_, 0, "kde toolbar widget"),
                    text_,
                    accel_,
                    0,
                    0,
                    parent_,
                    name_) {
  QLabel* label = static_cast<QLabel*>(widget());
  // asthetically, I want a space in front of and afterthe text
  label->setText(QString::fromLatin1(" ") + label->text());
  label->setBackgroundMode(Qt::PaletteButton);
  label->adjustSize();
}

// If compiling under KDE version < 3.1
#else
#include <qtoolbutton.h>
#include <qstyle.h>

// largely copied from KonqLabelAction
// Use a toolbutton instead of a label so it is styled correctly
class LabelAction::ToolBarLabel : public QToolButton {
public:
  ToolBarLabel(const QString& text_, QWidget* parent_ = 0, const char* name_ = 0)
    : QToolButton(parent_, name_) { setText(text_); }

protected:
  void drawButton(QPainter* p_) {
    // Draw the background
#if QT_VERSION >= 0x030100
    style().drawControl(QStyle::CE_DockWindowEmptyArea, p_, this, rect(), colorGroup(),
                        QStyle::Style_Enabled);
#else
    style().drawComplexControl(QStyle::CC_ToolButton, p_, this, rect(), colorGroup(),
                               QStyle::Style_Enabled, QStyle::SC_ToolButton);
#endif
    QRect r = QStyle::visualRect(style().subRect(QStyle::SR_ToolButtonContents, this), this);
    // Draw the label
    style().drawControl(QStyle::CE_ToolButtonLabel, p_, this, r, colorGroup(),
                        QStyle::Style_Enabled);
  }
};

LabelAction::LabelAction(const QString& text_, int accel_,
                         KActionCollection* parent_/*=0*/, const char* name_/*=0*/)
 : KAction(text_, accel_, parent_, name_), m_label(0) {
}

int LabelAction::plug(QWidget* widget_, int index_) {
  KToolBar* tb = static_cast<KToolBar *>(widget_);

  int id = KAction::getToolButtonID();

  // asthetically, I want a space in front of and after the text
  m_label = new ToolBarLabel(QString::fromLatin1(" ") + text(), tb);
  tb->insertWidget(id, m_label->width(), m_label, index_);

  addContainer(tb, id);

  connect(tb, SIGNAL(destroyed()),
          this, SLOT(slotDestroyed()));

  return containerCount() - 1;
}

void LabelAction::unplug(QWidget* widget_) {
  KToolBar* tb = static_cast<KToolBar *>(widget_);

  int idx = findContainer(tb);

  if(idx != -1) {
    tb->removeItem(itemId(idx));
    removeContainer(idx);
  }

  m_label = 0;
}
#endif // end KDE < 3.1

// largely copied from KonqComboAction
LineEditAction::LineEditAction(const QString& text_, int accel_,
                                   KActionCollection* parent_/*=0*/, const char* name_/*=0*/)
 : KAction(text_, accel_, parent_, name_), m_lineEdit(0L) {
}

int LineEditAction::plug(QWidget* widget_, int index_) {
  KToolBar* tb = static_cast<KToolBar *>(widget_);

  int id = KAction::getToolButtonID();

  m_lineEdit = new KLineEdit(tb);
  m_lineEdit->setFixedWidth(m_lineEdit->fontMetrics().maxWidth()*5);
  
  tb->insertWidget(id, m_lineEdit->width(), m_lineEdit, index_);

  connect(m_lineEdit, SIGNAL(textChanged(const QString&)),
          this, SIGNAL(textChanged(const QString&)));

  addContainer(tb, id);

  connect(tb, SIGNAL(destroyed()), this, SLOT(slotDestroyed()));

  emit plugged();
  return containerCount() - 1;
}

void LineEditAction::unplug(QWidget* widget_) {
  KToolBar* tb = static_cast<KToolBar *>(widget_);

  int idx = findContainer(widget_);

  if(idx != -1) {
    tb->removeItem(itemId(idx));
    removeContainer(idx);
  }

  m_lineEdit = 0L;
}

void LineEditAction::clear() {
  m_lineEdit->clear();
}