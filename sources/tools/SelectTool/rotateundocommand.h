#ifndef ROTATEUNDOCOMMAND_H
#define ROTATEUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/myitembase.h"

class RotateUndoCommand : public QUndoCommand
{
public:
    RotateUndoCommand(QList<MYItemBase *> items, double angle);
    void undo();
    void redo();

private:
    double m_angle;
    QList<MYItemBase *> m_items;
    QRectF itemsBoundingRect();
};

#endif // ROTATEUNDOCOMMAND_H
