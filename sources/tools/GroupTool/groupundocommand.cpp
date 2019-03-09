#include "groupundocommand.h"
#include "global.h"

GroupUndoCommand::GroupUndoCommand(QList<MYItemBase *> items){
    setText(QObject::tr("Group"));
    m_items = items;

    m_groupItem = new MYItemBase(0,true);
    m_groupItem->setItemName(QObject::tr("Group %1").arg(++qApp->currentTypon()->groupCounter));
}

GroupUndoCommand::GroupUndoCommand(QDataStream &stream, MYItemBase *group){
    setText(QObject::tr("Group"));
    m_groupItem = new MYItemBase(0,true);
    m_groupItem->restore(stream);
    if ( group )
        group->addToGroup(m_groupItem);
}

GroupUndoCommand::~GroupUndoCommand(){
    delete m_groupItem;
}

void GroupUndoCommand::undo(){
    qApp->currentTypon()->typonScene()->deleteGroup(m_groupItem);
}

void GroupUndoCommand::redo(){
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
