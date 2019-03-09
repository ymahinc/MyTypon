#ifndef DELETETRACKUNDOCOMMAND_H
#define DELETETRACKUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/trackitem/track.h"

class DeleteTrackUndoCommand : public QUndoCommand
{
public:
    DeleteTrackUndoCommand(Track *item);
    void undo();
    void redo();

private:
    Track *m_trackItem;
    QPointF m_pos;
    Layer *m_layer;
};

#endif // DELETETRACKUNDOCOMMAND_H
