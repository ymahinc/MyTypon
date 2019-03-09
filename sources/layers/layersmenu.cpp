#include "layersmenu.h"
#include "global.h"
#include "layer.h"
#include "layeraction.h"
#include "layersstack.h"
#include <QDebug>

LayersMenu::LayersMenu(QWidget *parent)
    : QMenu(parent){
    setTitle(QObject::tr("Send to layer"));
    connect(this,SIGNAL(aboutToShow()),this,SLOT(updateMenu()));
    m_actionGroup = new QActionGroup(this);
    connect(m_actionGroup,SIGNAL(triggered(QAction*)),this,SLOT(onActionTriggered(QAction*)));
}

void LayersMenu::updateMenu(){
    clear();
    if ( qApp->currentTypon() ){
        for(int i = qApp->currentTypon()->typonScene()->layers()->count() - 1; i > 0; i--){
            LayerAction *layerAction = new LayerAction(this, qApp->currentTypon()->typonScene()->layers()->at(i));
            addAction(layerAction);
            m_actionGroup->addAction(layerAction);
        }
    }
}

void LayersMenu::onActionTriggered(QAction *action){
    LayerAction *layerAction = dynamic_cast<LayerAction *>(action);
    if ( layerAction )
        emit sendToLayer(layerAction->layer());
}
