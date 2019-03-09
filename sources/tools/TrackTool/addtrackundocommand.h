#ifndef ADDTRACKUNDOCOMMAND_H
#define ADDTRACKUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/trackitem/track.h"

class AddTrackUndoCommand : public QUndoCommand
{
public:
    AddTrackUndoCommand(Track *track);
    AddTrackUndoCommand(QDataStream &stream, MYItemBase *group = 0);
    ~AddTrackUndoCommand();
    void undo();
    void redo();

private:
    Track *m_track;
    Layer *m_layer;
};

#endif // ADDTRACKUNDOCOMMAND_H
