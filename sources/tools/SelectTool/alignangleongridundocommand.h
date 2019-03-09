#ifndef ALIGNANGLEONGRIDUNDOCOMMAND_H
#define ALIGNANGLEONGRIDUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/trackitem/track.h"
#include "items/trackitem/angle.h"

class AlignAngleOnGridUndoCommand : public QUndoCommand
{
public:
    AlignAngleOnGridUndoCommand(Angle *angle, QPointF newPos);
    void undo();
    void redo();

private:
    QPointF m_oldPos;
    QPointF m_newPos;
    Angle *m_angle;
};

#endif // ALIGNANGLEONGRIDUNDOCOMMAND_H
