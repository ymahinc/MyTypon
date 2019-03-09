#ifndef EDITTRACKUNDOCOMMAND_H
#define EDITTRACKUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/trackitem/track.h"

class EditTrackUndoCommand : public QUndoCommand
{
public:
    EditTrackUndoCommand(Track *track,  QString itemName, double width, Track::TrackType type);
    ~EditTrackUndoCommand();
    void undo();
    void redo();
    int id() const;
    bool mergeWith(const QUndoCommand *command);
    Track *trackItem() const;
    Track *sourceItem() const;

private:
    Track m_oldTrackItem;
    Track *m_newTrackItem;
    Track *m_targetItem;
};

#endif // EDITTRACKUNDOCOMMAND_H
