#ifndef UNGROUPUNDOCOMMAND_H
#define UNGROUPUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/myitembase.h"

class UngroupUndoCommand : public QUndoCommand
{
public:
    UngroupUndoCommand(MYItemBase *item);
    void undo();
    void redo();

private:
    QList<MYItemBase *> m_items;
    MYItemBase *m_groupItem;
};

#endif // UNGROUPUNDOCOMMAND_H
