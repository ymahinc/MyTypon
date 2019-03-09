#ifndef EDITGROUPUNDOCOMMAND_H
#define EDITGROUPUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/myitembase.h"

class EditGroupUndoCommand : public QUndoCommand
{
public:
    EditGroupUndoCommand(MYItemBase *item, QString itemName);
    void undo();
    void redo();
    int id() const;
    bool mergeWith(const QUndoCommand *command);
    MYItemBase *sourceItem() const;
    QString newName() const;

private:
    MYItemBase *m_group;
    QString m_newName;
    QString m_oldName;
};


#endif // EDITGROUPUNDOCOMMAND_H
