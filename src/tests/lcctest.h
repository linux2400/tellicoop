/***************************************************************************
    copyright            : (C) 2009 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifndef LCCTEST_H
#define LCCTEST_H

#include <QObject>

class LccTest : public QObject {
Q_OBJECT

private Q_SLOTS:
  void testSorting();
};

#endif