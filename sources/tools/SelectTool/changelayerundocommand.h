#ifndef CHANGELAYERUNDOCOMMAND_H
#define CHANGELAYERUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/myitembase.h"
#include "items/paditem/padgroup.h"

class ChangeLayerUndoCommand : public QUndoCommand
{
public:
    ChangeLayerUndoCommand(MYItemBase *item, Layer *layer);
    void undo();
    void redo();

private:
    Layer *m_layer;
    Layer *m_oldLayer;
    MYItemBase *m_item;
    PadGroup *m_pad;
};

#endif // CHANGELAYERUNDOCOMMAND_H
