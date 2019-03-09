#ifndef MYUNITSSPINBOX_H
#define MYUNITSSPINBOX_H

/*
 * spin box wich displayed a raw value in selected unit: MM, In or mil
 */

#include <QDoubleSpinBox>
#include "mytyponunits.h"

class MYUnitsSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    MYUnitsSpinBox(QWidget *parent = Q_NULLPTR);
    void setRawValue(double value);
    double rawValue();
    void setAllowZero(bool allow);

signals:
    void rawValueChanged();

public slots:
    void setUnit();

private slots:
    void updateRawValue(double value);

private:
    double m_rawValue;
    double m_allowZero;
};

#endif // MYUNITSSPINBOX_H
