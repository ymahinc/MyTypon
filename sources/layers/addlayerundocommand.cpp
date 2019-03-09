#include "addlayerundocommand.h"
#include <QDebug>

AddLayerUndoCommand::AddLayerUndoCommand(LayersStack *stack, Layer *layer)
    : m_stack(stack), m_layer(layer){
    setText(QObject::tr("Add %1 layer").arg(layer->name()));

}

void AddLayerUndoCommand::undo(){
    m_stack->removeLayer(m_layer);
}

void AddLayerUndoCommand::redo(){
    m_stack->addLayer(m_layer);
}
