#include "movelayerundocommand.h"

#include <QDebug>

MoveLayerUndoCommand::MoveLayerUndoCommand(LayersStack *stack, Layer *layer, bool up)
    : m_stack(stack), m_layer(layer), m_up(up){
    if ( up )
        setText(QObject::tr("Move %1 layer up").arg(layer->name()));
    else
        setText(QObject::tr("Move %1 layer down").arg(layer->name()));
}

void MoveLayerUndoCommand::undo(){
    if ( m_up )
        m_stack->moveLayerDown(m_layer);
    else
       m_stack->moveLayerUp(m_layer);
}

void MoveLayerUndoCommand::redo(){
    if ( m_up )
        m_stack->moveLayerUp(m_layer);
    else
       m_stack->moveLayerDown(m_layer);
}
