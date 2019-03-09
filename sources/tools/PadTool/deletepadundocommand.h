#ifndef DELETEPADUNDOCOMMAND_H
#define DELETEPADUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/paditem/padgroup.h"

class DeletePadUndoCommand : public QUndoCommand
{
public:
    DeletePadUndoCommand(PadGroup *pad);
    void undo();
    void redo();

private:
    PadGroup *m_pad;
    QPointF m_pos;
    Layer *m_layer;
};

#endif // DELETEPADUNDOCOMMAND_H
