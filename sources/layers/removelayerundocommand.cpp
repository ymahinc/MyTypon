#include "removelayerundocommand.h"
#include <QDebug>
#include "global.h"

RemoveLayerUndoCommand::RemoveLayerUndoCommand(LayersStack *stack, Layer *layer, int index)
    : m_stack(stack), m_layer(layer), m_index(index){
    setText(QObject::tr("Remove layer %1").arg(layer->name()));
}

RemoveLayerUndoCommand::~RemoveLayerUndoCommand(){
    if ( !m_layer->scene() )
        delete m_layer;
}

void RemoveLayerUndoCommand::undo(){
    for ( int i = 0; i < m_layer->childItems().count(); i++ ){
        MYItemBase *it = dynamic_cast<MYItemBase *>(m_layer->childItems().at(i));
        if ( it )
            qApp->currentTypon()->typonScene()->addTyponItem(it,true);
    }
    m_stack->insertLayer(m_layer,m_index);
}

void RemoveLayerUndoCommand::redo(){

    // actual removing of items from scene is done by removing their parent: the layer
    m_stack->removeLayer(m_layer);

    // update scene typon items list actually removing items from scene
    for ( int i = 0; i < m_layer->childItems().count(); i++ ){
        MYItemBase *it = dynamic_cast<MYItemBase *>(m_layer->childItems().at(i));
        if ( it ){
            qApp->currentTypon()->typonScene()->removeTyponItem(it,true);
            it->setOffScreen(true);
        }

        MYItemBase *itGroup = it->group();
        // if group have no more childs, unselect it
        if ( itGroup ){
            if ( itGroup->allChildsAreUnavailable() )
                qApp->currentTypon()->typonScene()->removeSelectedItem(itGroup,true);
        }
    }
}
