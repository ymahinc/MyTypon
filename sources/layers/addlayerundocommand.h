#ifndef ADDLAYERUNDOCOMMAND_H
#define ADDLAYERUNDOCOMMAND_H

#include <QUndoCommand>
#include "layersstack.h"
#include "layer.h"

class AddLayerUndoCommand : public QUndoCommand
{

public:
    AddLayerUndoCommand(LayersStack *stack, Layer *layer);
    void undo();
    void redo();

private:
    LayersStack *m_stack;
    Layer *m_layer;
};

#endif // ADDLAYERUNDOCOMMAND_H
