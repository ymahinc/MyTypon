#include "editdrawundocommand.h"
#include "global.h"
#include "drawtool.h"

EditDrawUndoCommand::EditDrawUndoCommand(DrawItem *item, QString itemName, double width, DrawItem::FillType fillType, double groundPlaneOffset){
    if ( item->drawType() == DrawItem::Rectangle )
        setText(QObject::tr("Edit Rectangle"));
    if ( item->drawType() == DrawItem::Polygon )
        setText(QObject::tr("Edit Polygon"));
    if ( item->drawType() == DrawItem::Ellipse )
        setText(QObject::tr("Edit Ellipse"));

    m_targetItem = item;
    m_newDrawItem = new DrawItem();

    m_newDrawItem->setItemName(itemName);
    m_newDrawItem->setWidth(width);
    m_newDrawItem->setFillType(fillType);
    m_newDrawItem->setGroundPlaneOffset(groundPlaneOffset);

    m_oldDrawItem.setItemName(item->itemName());
    m_oldDrawItem.setWidth(item->width());
    m_oldDrawItem.setFillType(item->fillType());
    m_oldDrawItem.setGroundPlaneOffset(item->groundPlaneOffset());
}

EditDrawUndoCommand::~EditDrawUndoCommand(){
    delete m_newDrawItem;
}

int EditDrawUndoCommand::id() const{
    return 6;
}

bool EditDrawUndoCommand::mergeWith(const QUndoCommand *command){
    // if not an editTrack command return
    if (command->id() != id() )
        return false;
    const EditDrawUndoCommand *cmd = static_cast<const EditDrawUndoCommand*>(command);
    // if this is a new edit text command but not for this item, return
    if ( cmd->sourceItem() != m_targetItem )
        return false;
    //update new value with value of this command of the new command and accept merging
    m_newDrawItem->setItemName(cmd->drawItem()->itemName());
    m_newDrawItem->setWidth(cmd->drawItem()->width());
    m_newDrawItem->setFillType(cmd->drawItem()->fillType());
    m_newDrawItem->setGroundPlaneOffset(cmd->drawItem()->groundPlaneOffset());
    return true;
}

DrawItem *EditDrawUndoCommand::drawItem() const{
    return m_newDrawItem;
}

DrawItem *EditDrawUndoCommand::sourceItem() const{
    return m_targetItem;
}

void EditDrawUndoCommand::undo(){
    m_targetItem->setItemName(m_oldDrawItem.itemName());
    m_targetItem->setWidth(m_oldDrawItem.width());
    m_targetItem->setFillType(m_oldDrawItem.fillType());
    m_targetItem->setGroundPlaneOffset(m_oldDrawItem.groundPlaneOffset());
    // update widget values if there is one
    if ( m_targetItem->tool() )
         m_targetItem->tool()->updateTool();
    qApp->selectionTree()->updateName(m_targetItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_targetItem->groupBoundingRect(),m_targetItem->layer());
}

void EditDrawUndoCommand::redo(){
    m_targetItem->setItemName(m_newDrawItem->itemName());
    m_targetItem->setWidth(m_newDrawItem->width());
    m_targetItem->setFillType(m_newDrawItem->fillType());
    m_targetItem->setGroundPlaneOffset(m_newDrawItem->groundPlaneOffset());
    // update widget values if there is one
    if ( m_targetItem->tool() )
         m_targetItem->tool()->updateTool();
    qApp->selectionTree()->updateName(m_targetItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_targetItem->groupBoundingRect(),m_targetItem->layer());
}
