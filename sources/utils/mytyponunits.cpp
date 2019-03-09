#include "mytyponunits.h"
#include <QPointF>
#include <QDebug>

MYTyponUnits::MYTyponUnits(QObject *parent)
    : QObject(parent) {
    m_unit = MM;
}

void MYTyponUnits::setCurrentUnit(Units unit){
    m_unit = unit;
}

MYTyponUnits::Units MYTyponUnits::currentUnit(){
    return m_unit;
}

double MYTyponUnits::unitToScene(Units unit, double value){
    switch ( unit ) {
    case MM:
        return value * 39.3701;
        break;
    case INCHES:
        return value * 1000;
        break;
    case MIL:
        return value ;
        break;
    }
    return 0;
}

qint64 MYTyponUnits::unitToSceneRounded(Units unit, double value){
    switch ( unit ) {
    case MM:
        return qRound64(value * 39.3701);
        break;
    case INCHES:
        return qRound64(value * 1000.0);
        break;
    case MIL:
        return qRound64(value) ;
        break;
    }
    return 0;
}

double MYTyponUnits::sceneToUnitRounded(Units unit, double value){
    switch ( unit ) {
    case MM:
        return qRound64(value * 0.0254 * 1000.0) / 1000.0;
        break;
    case INCHES:
        return qRound64(value) / 1000.0;
        break;
    case MIL:
        return qRound64(value);
        break;
    }
    return 0;
}

QPoint MYTyponUnits::unitToScene(Units unit, QPointF value){
    QPoint pt;
    pt.setX(unitToScene(unit, value.x()));
    pt.setY(unitToScene(unit, value.y()));
    return pt;
}

double  MYTyponUnits::sceneToUnit(Units unit, double value){
    switch ( unit ) {
    case MM:
        return value * 0.0254;
        break;
    case INCHES:
        return value / 1000.0;
        break;
    case MIL:
        return value;
        break;
    }
    return 0;
}

QPointF MYTyponUnits::sceneToUnit(Units unit, QPointF value){
    QPointF pt;
    pt.setX(sceneToUnit(unit, value.x()));
    pt.setY(sceneToUnit(unit, value.y()));
    return pt;
}

QString MYTyponUnits::unitToString(Units unit){
    switch ( unit ) {
    case MM:
        return tr("mm");
        break;
    case INCHES:
        return tr("In");
        break;
    case MIL:
        return tr("Mil");
        break;
    }
    return QString();
}

double MYTyponUnits::convertToUnit(Units sourceUnit, Units destUnit, double value){
    double sceneVal = unitToScene(sourceUnit,value);
    return sceneToUnit(destUnit, sceneVal);
}

QPointF MYTyponUnits::convertToUnit(Units sourceUnit, Units destUnit, QPointF value){
    QPointF pt;
    pt.setX(convertToUnit(sourceUnit, destUnit, value.x()));
    pt.setY(convertToUnit(sourceUnit, destUnit, value.y()));
    return pt;
}


// returns new scene value in new unit from  original scene value old unit
double MYTyponUnits::convertToUnitScene(Units sourceUnit, Units destUnit, double valueInScenePos){
    double originUnitValue = sceneToUnit(sourceUnit, valueInScenePos);
    double destUnitValue = convertToUnit(sourceUnit,destUnit,originUnitValue);
    return unitToScene(destUnit, destUnitValue);
}

QPointF MYTyponUnits::convertToUnitScene(Units sourceUnit, Units destUnit, QPointF valueInScenePos){
    QPointF pt;
    pt.setX(convertToUnitScene(sourceUnit, destUnit, valueInScenePos.x()));
    pt.setY(convertToUnitScene(sourceUnit, destUnit, valueInScenePos.y()));
    return pt;
}
