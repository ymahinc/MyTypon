#ifndef MYUNITSCOMBOBOX_H
#define MYUNITSCOMBOBOX_H

/*
 * comboBox that allow to select a unit
 */

#include <QComboBox>
#include "mytyponunits.h"

class MYUnitsComboBox : public QComboBox
{
    Q_OBJECT

public:
    MYUnitsComboBox(QWidget *parent = Q_NULLPTR);
    void setCurrentUnit(MYTyponUnits::Units unit);
    MYTyponUnits::Units currentUnit();

private slots:
    void onIndexChanged(int index);
};

#endif // MYUNITSCOMBOBOX_H
