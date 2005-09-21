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

#ifndef TELLICO_BORROWER_H
#define TELLICO_BORROWER_H

#include "ptrvector.h"
#include "datavectors.h"

#include <ksharedptr.h>

#include <qdatetime.h>

namespace Tellico {
  namespace Data {

class Loan : public KShared {

public:
  Loan(Data::Entry* entry, const QDate& loanDate, const QDate& dueDate, const QString& note);
  Loan(const Loan& other);

  Data::Borrower* borrower() const { return m_borrower; }
  void setBorrower(Data::Borrower* b) { m_borrower = b; }

  const QString& uid() const { return m_uid; }
  void setUID(const QString& uid) { m_uid = uid; }

  Data::Entry* entry() const { return m_entry; }

  const QDate& loanDate() const { return m_loanDate; }

  const QDate& dueDate() const { return m_dueDate; }
  void setDueDate(const QDate& date) { m_dueDate = date; }

  const QString& note() const { return m_note; }
  void setNote(const QString& text) { m_note = text; }

  bool inCalendar() const { return m_inCalendar; }
  void setInCalendar(bool inCalendar) { m_inCalendar = inCalendar; }

private:
  Loan& operator=(const Loan&);

  QString m_uid;
  Data::BorrowerPtr m_borrower;
  Data::EntryPtr m_entry;
  QDate m_loanDate;
  QDate m_dueDate;
  QString m_note;
  bool m_inCalendar;
};

typedef KSharedPtr<Loan> LoanPtr;
typedef Vector<Loan> LoanVec;
typedef LoanVec::Iterator LoanVecIt;

/**
 * @author Robby Stephenson
 */
class Borrower : public KShared {

public:
//  Borrower() {}
  Borrower(const QString& name, const QString& uid);
  Borrower(const Borrower& other);
  Borrower& operator=(const Borrower& other);

//  bool isNull() const { return m_name.isEmpty(); }
  const QString& uid() const { return m_uid; }
  const QString& name() const { return m_name; }
  const LoanVec& loans() const { return m_loans; }
  bool isEmpty() const { return m_loans.isEmpty(); }

  Data::Loan* loan(const Data::Entry* entry);
  void addLoan(Data::Loan* loan);
  bool removeLoan(Data::Loan* loan);
//  bool removeLoan(Data::Entry* entry);

private:
  QString m_name;
  QString m_uid; // uid used by KABC
  LoanVec m_loans;
};

  } // end namespace
} // end namespace
#endif