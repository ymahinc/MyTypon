#ifndef MOVEANGLEUNDOCOMMAND_H
#define MOVEANGLEUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/trackitem/angle.h"
#include "items/trackitem/track.h"

class MoveAngleUndoCommand : public QUndoCommand
{
public:
    MoveAngleUndoCommand(Angle *angle, QPointF oldPos, QPointF pos);
    void undo();
    void redo();

private:
    QPointF m_pos;
    QPointF m_oldPos;
    Angle *m_angle;
    bool m_firstRedo;
};

#endif // MOVEANGLEUNDOCOMMAND_H
