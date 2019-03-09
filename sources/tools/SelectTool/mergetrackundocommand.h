#ifndef MERGETRACKUNDOCOMMAND_H
#define MERGETRACKUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/trackitem/angle.h"
#include "items/trackitem/track.h"

class MergeTrackUndoCommand : public QUndoCommand
{
public:
    MergeTrackUndoCommand(Angle *angle1, Angle *angle2);
    void undo();
    void redo();

private:
    Angle *m_angle1;
    Angle *m_angle2;
    Track *m_track1;
    Track *m_track2;
    int mergeAngleIndex;
    int m_mergeSense;
};

#endif // MERGETRACKUNDOCOMMAND_H
