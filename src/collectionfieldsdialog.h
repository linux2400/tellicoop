/***************************************************************************
    copyright            : (C) 2003-2004 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifndef BCCOLLECTIONFIELDSDIALOG_H
#define BCCOLLECTIONFIELDSDIALOG_H

class KComboBox;
class KLineEdit;
class KPushButton;

class QRadioButton;
class QCheckBox;
class QPainter;

#include "field.h"

#include <kdialogbase.h>

#include <qmap.h>
#include <qlistbox.h>

namespace Bookcase {
  namespace Data {
    class Collection;
  }

/**
 * BCListBoxText subclasses QListBoxText so that @ref setText() can be made public,
 * and the font color can be changed
 *
 * @author Robby Stephenson
 * @version $Id: collectionfieldsdialog.h 386 2004-01-24 05:12:28Z robby $
 */
class ListBoxText : public QListBoxText {
public:
  ListBoxText(QListBox* listbox, Data::Field* field);
  ListBoxText(QListBox* listbox, Data::Field* field, QListBoxItem* after);

  Data::Field* field() const { return m_field; }
  void setField(Data::Field* field) { m_field = field; }
  void setColored(bool colored);
  void setText(const QString& text);

protected:
  virtual void paint(QPainter* painter);

private:
  Data::Field* m_field;
  bool m_colored;
};

/**
 * @author Robby Stephenson
 * @version $Id: collectionfieldsdialog.h 386 2004-01-24 05:12:28Z robby $
 */
class CollectionFieldsDialog : public KDialogBase {
Q_OBJECT

public:
  /**
   * The constructor sets up the dialog.
   *
   * @param coll A pointer to the collection parent of all the attributes
   * @param parent A pointer to the parent widget
   * @param name The widget name
   */
  CollectionFieldsDialog(Data::Collection* coll, QWidget* parent, const char* name=0);
  ~CollectionFieldsDialog();

signals:
  void signalCollectionModified();

protected slots:
  virtual void slotOk();
  virtual void slotApply();
  virtual void slotDefault();
  void slotNew();
  void slotDelete();
  void slotMoveUp();
  void slotMoveDown();
  void slotTypeChanged(const QString& type);
  void slotHighlightedChanged(int index);
  void slotModified();
  void slotUpdateTitle(const QString& title);
  void slotShowExtendedProperties();

protected:
  void updateField();
  ListBoxText* findItem(const QListBox* box, const Data::Field* field);

private:
  Data::Collection* m_coll;
  Data::Collection* m_defaultCollection;
  QMap<Data::Field::FieldType, QString> m_typeMap;
  Data::FieldList m_copiedFields;
  Data::FieldList m_newFields;
  Data::Field* m_currentField;
  int m_currentListItem;
  bool m_modified;
  bool m_updatingValues;
  bool m_reordered;
  
  QListBox* m_fieldsBox;
  KPushButton* m_btnNew;
  KPushButton* m_btnDelete;
  KPushButton* m_btnUp;
  KPushButton* m_btnDown;
  
  KLineEdit* m_titleEdit;
  KComboBox* m_typeCombo;
  KLineEdit* m_allowEdit;
  KComboBox* m_catCombo;
  KLineEdit* m_descEdit;
  KPushButton* m_btnExtended;

  QRadioButton* m_formatNone;
  QRadioButton* m_formatPlain;
  QRadioButton* m_formatTitle;
  QRadioButton* m_formatName;
  QCheckBox* m_complete;
  QCheckBox* m_multiple;
  QCheckBox* m_grouped;
};

} // end namespace
#endif