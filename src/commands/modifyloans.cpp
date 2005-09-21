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

#include "modifyloans.h"
#include "../document.h"
#include "../entry.h"
#include "../controller.h"
#include "../calendarhandler.h"

#include <klocale.h>

using Tellico::Command::ModifyLoans;

ModifyLoans::ModifyLoans(Data::Loan* oldLoan_, Data::Loan* newLoan_, bool addToCalendar_)
    : KCommand()
    , m_oldLoan(oldLoan_)
    , m_newLoan(newLoan_)
    , m_addToCalendar(addToCalendar_)
{
}

void ModifyLoans::execute() {
  if(!m_oldLoan || !m_newLoan) {
    return;
  }

  Data::Borrower* b = m_oldLoan->borrower();
  b->removeLoan(m_oldLoan);
  b->addLoan(m_newLoan);
  Controller::self()->modifiedBorrower(b);

  if(m_addToCalendar && !m_oldLoan->inCalendar()) {
    Data::LoanVec loans;
    loans.append(m_newLoan);
    CalendarHandler::addLoans(loans);
  } else if(!m_addToCalendar && m_oldLoan->inCalendar()) {
    Data::LoanVec loans;
    loans.append(m_newLoan); // CalendarHandler checks via uid
    CalendarHandler::removeLoans(loans);
  }
}

void ModifyLoans::unexecute() {
  if(!m_oldLoan || !m_newLoan) {
    return;
  }

  Data::Borrower* b = m_oldLoan->borrower();
  b->removeLoan(m_newLoan);
  b->addLoan(m_oldLoan);
  Controller::self()->modifiedBorrower(b);

  if(m_addToCalendar && !m_oldLoan->inCalendar()) {
    Data::LoanVec loans;
    loans.append(m_newLoan);
    CalendarHandler::removeLoans(loans);
  } else if(!m_addToCalendar && m_oldLoan->inCalendar()) {
    Data::LoanVec loans;
    loans.append(m_oldLoan);
    CalendarHandler::addLoans(loans);
  }
}

QString ModifyLoans::name() const {
  return i18n("Modify Loan");
}