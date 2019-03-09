#ifndef SPLITTRACKUNDOCOMMAND_H
#define SPLITTRACKUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/trackitem/angle.h"
#include "items/trackitem/track.h"

class SplitTrackUndoCommand : public QUndoCommand
{
public:
    SplitTrackUndoCommand(Angle *angle);
    ~SplitTrackUndoCommand();
    void undo();
    void redo();

private:
    Angle *m_angle;
    Track *m_track;
    Track *m_newTrack;
    Angle *m_newFirstAngle;
};

#endif // SPLITTRACKUNDOCOMMAND_H
