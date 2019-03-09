#include "myunitscombobox.h"
#include "global.h"

MYUnitsComboBox::MYUnitsComboBox(QWidget *parent)
    : QComboBox(parent){
    addItem(MYTyponUnits::unitToString(MYTyponUnits::MM));
    addItem(MYTyponUnits::unitToString(MYTyponUnits::INCHES));
    addItem(MYTyponUnits::unitToString(MYTyponUnits::MIL));
    connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(onIndexChanged(int)));
    setCurrentUnit(qApp->workSpace()->unit);
}

void MYUnitsComboBox::onIndexChanged(int index){
    qApp->workSpace()->onUnitChanged((MYTyponUnits::Units)index);
}

void MYUnitsComboBox::setCurrentUnit(MYTyponUnits::Units unit){
    int index = (int) unit;
    if ( index < 3 )
        setCurrentIndex(index);
}

MYTyponUnits::Units MYUnitsComboBox::currentUnit(){
    return (MYTyponUnits::Units)currentIndex();
}
