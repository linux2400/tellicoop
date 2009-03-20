/***************************************************************************
    copyright            : (C) 2005-2006 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifndef TELLICO_CALENDARHANDLER_H
#define TELLICO_CALENDARHANDLER_H

#include <config.h>
#include "../borrower.h"

namespace KCal {
  class CalendarResources;
  class Todo;
}

namespace Tellico {

/**
 * @author Robby Stephenson
 */
class CalendarHandler {
public:
  static void addLoans(Data::LoanList loans);
  static void modifyLoans(Data::LoanList loans);
  static void removeLoans(Data::LoanList loans);

private:
  static QString timezone();

#ifdef HAVE_KCAL
  // helper function
  static void addLoans(Data::LoanList loans, KCal::CalendarResources* resources);
  static bool checkCalendar(KCal::CalendarResources* resources);
  static void populateTodo(KCal::Todo* todo, Data::LoanPtr loan);
#endif
};

} // end namespace

#endif