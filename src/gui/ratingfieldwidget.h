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

#ifndef RATINGFIELDWIDGET_H
#define RATINGFIELDWIDGET_H

#include "fieldwidget.h"

namespace Tellico {
  namespace GUI {
    class RatingWidget;

/**
 * @author Robby Stephenson
 */
class RatingFieldWidget : public FieldWidget {
Q_OBJECT

public:
  RatingFieldWidget(const Data::Field* field, QWidget* parent, const char* name=0);
  virtual ~RatingFieldWidget() {}

  virtual QString text() const;
  virtual void setText(const QString& text);

public slots:
  virtual void clear();

protected:
  virtual QWidget* widget();
  virtual void updateFieldHook(Data::Field* oldField, Data::Field* newField);

private:
  RatingWidget* m_rating;
};

  } // end GUI namespace
} // end namespace
#endif