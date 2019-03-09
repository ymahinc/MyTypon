#ifndef DELETEDRAWUNDOCOMMAND_H
#define DELETEDRAWUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/drawitem.h"

class DeleteDrawUndoCommand : public QUndoCommand
{
public:
    DeleteDrawUndoCommand(DrawItem *item);
    void undo();
    void redo();

private:
    DrawItem *m_drawItem;
    QPointF m_pos;
    Layer *m_layer;
};

#endif // DELETEDRAWUNDOCOMMAND_H
