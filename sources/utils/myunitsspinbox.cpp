#include "myunitsspinbox.h"
#include "global.h"
#include <QDebug>

MYUnitsSpinBox::MYUnitsSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent){
    m_rawValue = 1;
    setUnit();
    setAllowZero(false);
    connect(this,SIGNAL(valueChanged(double)),this,SLOT(updateRawValue(double)));
    connect(qApp->workSpace(),SIGNAL(workspaceUnitChanged()),this,SLOT(setUnit()));  
}

//update current unit
void MYUnitsSpinBox::setUnit(){
    // do  not recalculate rawValue on displayed value changed
    disconnect(this,SIGNAL(valueChanged(double)),this,SLOT(updateRawValue(double)));
    // allow all values with 3 decimals
    setDecimals(3);
    setMinimum(0.000);
    setMaximum(50000);
    // get the value from the new unit from rawValue
    setValue(MYTyponUnits::sceneToUnitRounded(qApp->workSpace()->unit, m_rawValue));
    // update spinbox boundaries according to new item and if a value of zero is allowed
    switch ( qApp->workSpace()->unit ) {
    case MYTyponUnits::MM:{
        setDecimals(3);
        setMinimum(m_allowZero == true ? 0.0 : 0.0254);
        setMaximum(1270.0);
        setSuffix(tr(" mm"));
        setSingleStep(0.127);
        break;}
    case MYTyponUnits::INCHES:{
        setDecimals(3);
        setMinimum(m_allowZero == true ? 0.0 : 0.001);
        setMaximum(50);
        setSuffix(tr(" In"));
        setSingleStep(0.005);
        break;}
    case MYTyponUnits::MIL:{
        setMinimum(m_allowZero == true ? 0.0 : 1.0);
        setMaximum(50000);
        setDecimals(0);
        setSuffix(tr(" mil"));
        setSingleStep(5);
        break;}
    }
    // reactivate rawValue recompute when displayed value has changed
    connect(this,SIGNAL(valueChanged(double)),this,SLOT(updateRawValue(double)));
}

void MYUnitsSpinBox::setRawValue(double value){
    m_rawValue = value;
    setUnit();
    emit rawValueChanged();
}

double MYUnitsSpinBox::rawValue(){
    return m_rawValue;
}

void MYUnitsSpinBox::updateRawValue(double value){
    m_rawValue = MYTyponUnits::unitToScene(qApp->workSpace()->unit,value);
    emit rawValueChanged();
}

void MYUnitsSpinBox::setAllowZero(bool allow){
    m_allowZero = allow;
}

