#ifndef DELETETEXTUNDOCOMMAND_H
#define DELETETEXTUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/textitem/textitem.h"

class DeleteTextUndoCommand : public QUndoCommand
{
public:
    DeleteTextUndoCommand(TextItem *item);
    void undo();
    void redo();

private:
    TextItem *m_textItem;
    QPointF m_pos;
    Layer *m_layer;
};

#endif // DELETETEXTUNDOCOMMAND_H
