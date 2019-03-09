#include "myunitslabel.h"
#include "global.h"

MYUnitsLabel::MYUnitsLabel(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent,f){
    clearText();
    m_prefix ="";
    m_rawValue = -1;
    connect(qApp->workSpace(),SIGNAL(workspaceUnitChanged()),this,SLOT(onUnitChanged()));
}

void MYUnitsLabel::setRawValue(double value){
    m_rawValue = value;
    setText(tr("%1 %2 %3").arg(m_prefix)
            .arg(MYTyponUnits::sceneToUnit(qApp->workSpace()->unit,value))
            .arg(MYTyponUnits::unitToString(qApp->workSpace()->unit)));
}

void MYUnitsLabel::onUnitChanged(){
    setRawValue(m_rawValue);
}

void MYUnitsLabel::clearText(){
    setText(tr("%1 --.--- %2").arg(m_prefix).arg(MYTyponUnits::unitToString(qApp->workSpace()->unit)));
}

void MYUnitsLabel::setPrefix(QString prefix){
    m_prefix = prefix;
}
