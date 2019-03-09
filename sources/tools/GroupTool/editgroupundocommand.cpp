#include "editgroupundocommand.h"
#include <QDebug>
#include "global.h"

EditGroupUndoCommand::EditGroupUndoCommand(MYItemBase *item, QString itemName){
    setText(QObject::tr("Edit Group"));
    m_group = item;
    m_oldName = item->itemName();
    m_newName = itemName;
}

void EditGroupUndoCommand::undo(){
    m_group->setItemName(m_oldName);
    if ( m_group->groupTool() )
         m_group->groupTool()->updateGroup();
    qApp->selectionTree()->updateName(m_group);
}

void EditGroupUndoCommand::redo(){
    m_group->setItemName(m_newName);
    if ( m_group->groupTool() )
         m_group->groupTool()->updateGroup();
    qApp->selectionTree()->updateName(m_group);
}

QString EditGroupUndoCommand::newName() const{
    return m_newName;
}

MYItemBase *EditGroupUndoCommand::sourceItem() const{
    return m_group;
}

int EditGroupUndoCommand::id() const{
    return 4;
}

// merge with previous edit undo command
bool EditGroupUndoCommand::mergeWith(const QUndoCommand *command){
    // if not an editText command return
    if (command->id() != id() )
        return false;
    const EditGroupUndoCommand *cmd = static_cast<const EditGroupUndoCommand*>(command);
    // if this is a new edit group command but not for this item, return
    if ( cmd->sourceItem() != m_group )
        return false;
    //update new value with value of this command of the new command and accept merging
    m_newName = cmd->newName();
    return true;
}

