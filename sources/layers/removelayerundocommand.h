#ifndef REMOVELAYERUNDOCOMMAND_H
#define REMOVELAYERUNDOCOMMAND_H

#include <QUndoCommand>
#include "layersstack.h"
#include "layer.h"

class RemoveLayerUndoCommand : public QUndoCommand
{

public:
    RemoveLayerUndoCommand(LayersStack *stack, Layer *layer, int index);
    ~RemoveLayerUndoCommand();
    void undo();
    void redo();

private:
    LayersStack *m_stack;
    Layer *m_layer;
    int m_index;
};

#endif // REMOVELAYERUNDOCOMMAND_H
