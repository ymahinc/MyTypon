#ifndef EDITLAYERUNDOCOMMAND_H
#define EDITLAYERUNDOCOMMAND_H

#include <QUndoCommand>
#include "layer.h"
#include "layersstack.h"

class EditLayerUndoCommand : public QUndoCommand
{

public:
    EditLayerUndoCommand(LayersStack *stack, Layer *layer, QColor newColor, QString newName);
    void undo();
    void redo();

private:
    LayersStack *m_stack;
    Layer *m_layer;
    QColor m_oldColor;
    QString m_oldName;
    QColor m_newColor;
    QString m_newName;
};

#endif // EDITLAYERUNDOCOMMAND_H
