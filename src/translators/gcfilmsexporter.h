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

#ifndef TELLICO_EXPORT_GCFILMSEXPORTER_H
#define TELLICO_EXPORT_GCFILMSEXPORTER_H

class QTextOStream;

#include "exporter.h"

namespace Tellico {
  namespace Export {

/**
 * @author Robby Stephenson
 */
class GCfilmsExporter : public Exporter {
Q_OBJECT

public:
  GCfilmsExporter();

  virtual bool exec();
  virtual QString formatString() const;
  virtual QString fileFilter() const;

  // no options
  virtual QWidget* widget(QWidget*, const char*) { return 0; }
  virtual void readOptions(KConfig*) {}
  virtual void saveOptions(KConfig*) {}

private:
  void push(QTextOStream& ts, QCString fieldName, Data::EntryVec::ConstIterator entry, bool format);
};

  } // end namespace
} // end namespace
#endif