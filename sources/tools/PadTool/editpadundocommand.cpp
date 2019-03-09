#include "editpadundocommand.h"
#include "tools/PadTool/padtool.h"
#include "global.h"

EditPadUndoCommand::EditPadUndoCommand(PadGroup *item, QString itemName, PadItem::PadType padType, double width,
                                       bool hasHole, double holeWidth, bool isVia){

    setText(QObject::tr("Edit Pad"));
    m_targetItem = item;
    m_newPadItem = new PadGroup();

    m_newPadItem->setItemName(itemName);
    m_newPadItem->setType(padType);
    m_newPadItem->pad()->setWidth(width);
    m_newPadItem->setHoleWidth(holeWidth);
    m_newPadItem->setHasHole(hasHole);
    m_newPadItem->setVia(isVia);

    m_oldPadItem.setItemName(item->itemName());
    m_oldPadItem.setType(item->pad()->padType());
    m_oldPadItem.pad()->setWidth(item->pad()->width());
    m_oldPadItem.setHoleWidth(item->hole()->width());
    m_oldPadItem.setHasHole(item->hasHole());
    m_oldPadItem.setVia(item->isVia());
}


EditPadUndoCommand::~EditPadUndoCommand(){
    delete m_newPadItem;
}

// restore old values for the item
void EditPadUndoCommand::undo(){
    m_targetItem->setItemName(m_oldPadItem.itemName());
    m_targetItem->setType(m_oldPadItem.pad()->padType());
    m_targetItem->pad()->setWidth(m_oldPadItem.pad()->width());
    m_targetItem->setHoleWidth(m_oldPadItem.hole()->width());
    m_targetItem->setHasHole(m_oldPadItem.hasHole());
    m_targetItem->setVia(m_oldPadItem.isVia());

    // update widget values if there is one
    if ( m_targetItem->tool() )
         m_targetItem->tool()->updateTool();
    qApp->selectionTree()->updateName(m_targetItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_targetItem->groupBoundingRect());
}

// restore new values for the item
void EditPadUndoCommand::redo(){
    m_targetItem->setItemName(m_newPadItem->itemName());
    m_targetItem->setType(m_newPadItem->pad()->padType());
    m_targetItem->pad()->setWidth(m_newPadItem->pad()->width());
    m_targetItem->setHoleWidth(m_newPadItem->hole()->width());
    m_targetItem->setHasHole(m_newPadItem->hasHole());
    m_targetItem->setVia(m_newPadItem->isVia());
    // update widget values if there is one
    if ( m_targetItem->tool() )
         m_targetItem->tool()->updateTool();
    qApp->selectionTree()->updateName(m_targetItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_targetItem->groupBoundingRect());
}


PadGroup *EditPadUndoCommand::padGroup() const{
    return m_newPadItem;
}

PadGroup *EditPadUndoCommand::sourceItem() const{
    return m_targetItem;
}

int EditPadUndoCommand::id() const{
    return 3;
}

// merge with previous edit undo command
bool EditPadUndoCommand::mergeWith(const QUndoCommand *command){
    // if not an editText command return
    if (command->id() != id() )
        return false;
    const EditPadUndoCommand *cmd = static_cast<const EditPadUndoCommand*>(command);
    // if this is a new edit text command but not for this item, return
    if ( cmd->sourceItem() != m_targetItem )
        return false;
    //update new value with value of this command of the new command and accept merging
    m_newPadItem->setItemName(cmd->padGroup()->itemName());
    m_newPadItem->setType(cmd->padGroup()->pad()->padType());
    m_newPadItem->pad()->setWidth(cmd->padGroup()->pad()->width());
    m_newPadItem->hole()->setWidth(cmd->padGroup()->hole()->width());
    m_newPadItem->setHasHole(cmd->padGroup()->hasHole());
    m_newPadItem->setVia(cmd->padGroup()->isVia());
    return true;
}
