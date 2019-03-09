#ifndef MOVELAYERUNDOCOMMAND_H
#define MOVELAYERUNDOCOMMAND_H

#include <QUndoCommand>
#include "layer.h"
#include "layersstack.h"

class MoveLayerUndoCommand : public QUndoCommand
{
public:
    MoveLayerUndoCommand(LayersStack *stack, Layer *layer, bool up);
    void undo();
    void redo();

private:
    LayersStack *m_stack;
    Layer *m_layer;
    bool m_up;
};

#endif // MOVELAYERUNDOCOMMAND_H
