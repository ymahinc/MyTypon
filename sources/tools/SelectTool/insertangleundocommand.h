#ifndef INSERTANGLEUNDOCOMMAND_H
#define INSERTANGLEUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/trackitem/track.h"
#include "items/trackitem/angle.h"

class InsertAngleUndoCommand : public QUndoCommand
{
public:
    InsertAngleUndoCommand(Track *track,QPointF pos);
    ~InsertAngleUndoCommand();
    void undo();
    void redo();

private:
    QPointF m_pos;
    Angle *m_angle;
    Track *m_track;
    int m_index;
    bool angleIsOwnedByTrack;
};

#endif // INSERTANGLEUNDOCOMMAND_H
