#include "ungroupundocommand.h"
#include "global.h"

UngroupUndoCommand::UngroupUndoCommand(MYItemBase *item){
    setText(QObject::tr("Ungroup"));
    m_groupItem = item;
    m_items = item->childs();
}

void UngroupUndoCommand::undo(){
    // if one of the group child was selected, group will be selected after creation
    bool atLeastOneChildWasSelected = false;
    // if not a temporary group, remove selected childs from selection list
    foreach(MYItemBase *it, m_items){
        if ( it->typonItemIsSelected() )
            atLeastOneChildWasSelected = true;
        m_groupItem->addToGroup(it);
        qApp->currentTypon()->typonScene()->removeSelectedItem(it,false);
    }

    // if not temporary grp is a new selected item
    if ( atLeastOneChildWasSelected )
        qApp->currentTypon()->typonScene()->addSelectedItem(m_groupItem);
}

void UngroupUndoCommand::redo(){
    qApp->currentTypon()->typonScene()->deleteGroup(m_groupItem);
}
