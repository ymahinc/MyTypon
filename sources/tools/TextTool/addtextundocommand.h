#ifndef ADDTEXTUNDOCOMMAND_H
#define ADDTEXTUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/textitem/textitem.h"
#include <QDataStream>

class AddTextUndoCommand : public QUndoCommand
{
public:
    AddTextUndoCommand(TextItem *item);
    AddTextUndoCommand(QDataStream &stream, MYItemBase *group = 0);
    ~AddTextUndoCommand();
    void undo();
    void redo();

private:
    TextItem *m_textItem;
    QPointF m_pos;
    Layer *m_layer;
};

#endif // ADDTEXTUNDOCOMMAND_H
