#ifndef ALIGNONGRIDUNDOCOMMAND_H
#define ALIGNONGRIDUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/myitembase.h"

class AlignOnGridUndoCommand : public QUndoCommand
{
public:
    AlignOnGridUndoCommand(MYItemBase *item);
    void undo();
    void redo();

private:
    //QPointF m_pos;
    QPointF m_oldPos;
    MYItemBase *m_item;
};

#endif // ALIGNONGRIDUNDOCOMMAND_H
