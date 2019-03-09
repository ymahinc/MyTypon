#ifndef ADDPADUNDOCOMMAND_H
#define ADDPADUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/paditem/padgroup.h"

class AddPadUndoCommand : public QUndoCommand
{
public:
    AddPadUndoCommand(PadGroup *pad, QPointF pos, Layer *layer);
    AddPadUndoCommand(QDataStream &stream, MYItemBase *group = 0);
    ~AddPadUndoCommand();
    void undo();
    void redo();

private:
    PadGroup *m_pad;
    Layer *m_layer;
    QPointF m_pos;
    double m_rotation;
};

#endif // ADDPADUNDOCOMMAND_H
