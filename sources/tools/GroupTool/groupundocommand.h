#ifndef GROUPUNDOCOMMAND_H
#define GROUPUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/myitembase.h"

class GroupUndoCommand : public QUndoCommand
{
public:
    GroupUndoCommand(QList<MYItemBase *> items);
    GroupUndoCommand(QDataStream &stream, MYItemBase *group);
    ~GroupUndoCommand();
    void undo();
    void redo();

private:
    QList<MYItemBase *> m_items;
    MYItemBase *m_groupItem;
};

#endif // GROUPUNDOCOMMAND_H
