#ifndef MYTYPONUNITS_H
#define MYTYPONUNITS_H

// class with static methods to convert unit values to scene values, scene values to unities
// get unit representative string
// scene value is based on mil eg: one in scene value is one mil

#include <QtGlobal>
#include <QString>
#include <QObject>

class MYTyponUnits : public QObject
{
    Q_OBJECT

public:
    static enum Units{ MM, INCHES, MIL } unit;

    MYTyponUnits(QObject *parent = 0);
    Units currentUnit();
    static double unitToScene(Units unit, double value);
    static qint64 unitToSceneRounded(Units unit, double value);
    static QPoint unitToScene(Units unit, QPointF value);
    static double sceneToUnit(Units unit, double value);
    static double sceneToUnitRounded(Units unit, double value);
    static QPointF sceneToUnit(Units unit, QPointF value);
    static double convertToUnit(Units sourceUnit, Units destUnit, double value);
    static QPointF convertToUnit(Units sourceUnit, Units destUnit, QPointF value);
    static double convertToUnitScene(Units sourceUnit, Units destUnit, double valueInScenePos);
    static QPointF convertToUnitScene(Units sourceUnit, Units destUnit, QPointF valueInScenePos);
    static QString unitToString(Units unit);

public slots:
    void setCurrentUnit(Units unit);

private:
    Units m_unit;
};

#endif // MYTYPONUNITS_H
