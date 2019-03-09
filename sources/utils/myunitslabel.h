#ifndef MYUNITSLABEL_H
#define MYUNITSLABEL_H

/*
 * Label that display a rawValue in current unit - used in statusBar
 */

#include <QLabel>
#include "mytyponunits.h"

class MYUnitsLabel : public QLabel
{
    Q_OBJECT
public:
    MYUnitsLabel(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    void setRawValue(double value);
    void setPrefix(QString prefix);

public slots:
    void clearText();

private slots:
    void onUnitChanged();

private:
    bool m_rawValue;
    QString m_prefix;
};

#endif // MYUNITSLABEL_H
