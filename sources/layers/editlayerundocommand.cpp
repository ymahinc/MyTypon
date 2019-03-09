#include "editlayerundocommand.h"
#include <QDebug>

EditLayerUndoCommand::EditLayerUndoCommand(LayersStack *stack, Layer *layer, QColor newColor, QString newName)
    : m_stack(stack), m_layer(layer), m_newColor(newColor), m_newName(newName){
    setText(QObject::tr("Edit %1 layer").arg(layer->name()));
    m_oldColor = layer->color();
    m_oldName = layer->name();
}

void EditLayerUndoCommand::undo(){
    m_stack->editLayer(m_layer,m_oldName,m_oldColor);
}

void EditLayerUndoCommand::redo(){
    m_stack->editLayer(m_layer,m_newName,m_newColor);
}
