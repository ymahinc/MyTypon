#include "myunitsmenulabel.h"
#include "../global.h"
#include <QDebug>

MYUnitsMenuLabel::MYUnitsMenuLabel(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent,f){

    //create menu
    m_menu = new QMenu(this);
    m_unitsActionGroup =  new QActionGroup(this);
    m_mmAction = new QAction(MYTyponUnits::unitToString(MYTyponUnits::MM),this);
    m_mmAction->setCheckable(true);
    m_inAction = new QAction(MYTyponUnits::unitToString(MYTyponUnits::INCHES),this);
    m_inAction->setCheckable(true);
    m_milAction = new QAction(MYTyponUnits::unitToString(MYTyponUnits::MIL),this);
    m_milAction->setCheckable(true);
    m_unitsActionGroup->addAction(m_mmAction);
    m_unitsActionGroup->addAction(m_inAction);
    m_unitsActionGroup->addAction(m_milAction);
    m_inAction->setChecked(true);
    m_menu->addAction(m_mmAction);
    m_menu->addAction(m_inAction);
    m_menu->addAction(m_milAction);
    setAlignment(Qt::AlignCenter);
    setFixedWidth(23);
    onUnitChanged();
    connect(m_unitsActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(onMenuChanged(QAction*)));
    // update selected unit when unit is modified from another widget of the application
    connect(qApp->workSpace(),SIGNAL(workspaceUnitChanged()),this,SLOT(onUnitChanged()));
}

void MYUnitsMenuLabel::mousePressEvent(QMouseEvent *ev){
    m_menu->exec(ev->globalPos());
}

// update selected action according to the new unit
void  MYUnitsMenuLabel::onUnitChanged(){
    disconnect(m_unitsActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(onMenuChanged(QAction*)));
    setText(MYTyponUnits::unitToString(qApp->workSpace()->unit));
    switch( qApp->workSpace()->unit ){
        case MYTyponUnits::MM:
            m_mmAction->setChecked(true);
            break;
        case MYTyponUnits::INCHES:
            m_inAction->setChecked(true);
            break;
        case MYTyponUnits::MIL:
            m_milAction->setChecked(true);
            break;
    }
    connect(m_unitsActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(onMenuChanged(QAction*)));
}

// update current unit according to the selected one
// and update the workspace
void MYUnitsMenuLabel::onMenuChanged(QAction *action){
    if ( action == m_mmAction ){
        if ( qApp->workSpace()->unit == MYTyponUnits::MM )
            return;
        setText(MYTyponUnits::unitToString(MYTyponUnits::MM));
        qApp->workSpace()->onUnitChanged(MYTyponUnits::MM);
    }
    if ( action == m_inAction ){
        if ( qApp->workSpace()->unit == MYTyponUnits::INCHES )
            return;
        setText(MYTyponUnits::unitToString(MYTyponUnits::INCHES));
        qApp->workSpace()->onUnitChanged(MYTyponUnits::INCHES);
    }
    if ( action == m_milAction ){
        if ( qApp->workSpace()->unit == MYTyponUnits::MIL )
            return;
        setText(MYTyponUnits::unitToString(MYTyponUnits::MIL));
        qApp->workSpace()->onUnitChanged(MYTyponUnits::MIL);
    }
}

// hide if rulers are hidden
void MYUnitsMenuLabel::changeVisibility(){
    setVisible(qApp->workSpace()->showRulers);
}
