#ifndef MOVEUNDOCOMMAND_H
#define MOVEUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/myitembase.h"

class MoveUndoCommand : public QUndoCommand
{
public:
    MoveUndoCommand(QList<MYItemBase *> items, QPointF delta);
    void undo();
    void redo();

private:
    QPointF m_delta;
    QList<MYItemBase *> m_items;
    bool m_firstRedo;
    QRectF itemsBoundingRect();
};

#endif // MOVEUNDOCOMMAND_H
