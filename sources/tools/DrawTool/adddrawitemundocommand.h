#ifndef ADDDRAWITEMUNDOCOMMAND_H
#define ADDDRAWITEMUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/drawitem.h"

class AddDrawItemUndoCommand : public QUndoCommand
{
public:
    AddDrawItemUndoCommand(DrawItem *item);
    AddDrawItemUndoCommand(QDataStream &stream, MYItemBase *group = 0);
    ~AddDrawItemUndoCommand();
    void undo();
    void redo();

private:
    DrawItem *m_drawItem;
    Layer *m_layer;
};

#endif // ADDDRAWITEMUNDOCOMMAND_H
