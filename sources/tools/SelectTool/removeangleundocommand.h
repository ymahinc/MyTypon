#ifndef REMOVEANGLEUNDOCOMMAND_H
#define REMOVEANGLEUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/trackitem/track.h"

class RemoveAngleUndoCommand : public QUndoCommand
{
public:
    RemoveAngleUndoCommand(Angle *angle);
    ~RemoveAngleUndoCommand();
    void undo();
    void redo();

private:
    QPointF m_pos;
    Angle *m_angle;
    Track *m_track;
    int m_index;
};

#endif // REMOVEANGLEUNDOCOMMAND_H
