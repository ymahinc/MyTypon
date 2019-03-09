#include "edittrackundocommand.h"
#include "global.h"
#include "tools/TrackTool/tracktool.h"

EditTrackUndoCommand::EditTrackUndoCommand(Track *track,  QString itemName, double width, Track::TrackType type){
    setText(QObject::tr("Edit Track"));
    m_targetItem = track;
    m_newTrackItem = new Track();

    m_newTrackItem->setItemName(itemName);
    m_newTrackItem->setWidth(width);
    m_newTrackItem->setType(type);

    m_oldTrackItem.setItemName(track->itemName());
    m_oldTrackItem.setWidth(track->width());
    m_oldTrackItem.setType(track->trackType());
}

EditTrackUndoCommand::~EditTrackUndoCommand(){
    delete m_newTrackItem;
}

int EditTrackUndoCommand::id() const{
    return 5;
}

bool EditTrackUndoCommand::mergeWith(const QUndoCommand *command){
    // if not an editTrack command return
    if (command->id() != id() )
        return false;
    const EditTrackUndoCommand *cmd = static_cast<const EditTrackUndoCommand*>(command);
    // if this is a new edit text command but not for this item, return
    if ( cmd->sourceItem() != m_targetItem )
        return false;
    //update new value with value of this command of the new command and accept merging
    m_newTrackItem->setItemName(cmd->trackItem()->itemName());
    m_newTrackItem->setWidth(cmd->trackItem()->width());
    m_newTrackItem->setType(cmd->trackItem()->trackType());
    return true;
}

Track *EditTrackUndoCommand::trackItem() const{
    return m_newTrackItem;
}

Track *EditTrackUndoCommand::sourceItem() const{
    return m_targetItem;
}

void EditTrackUndoCommand::undo(){
    m_targetItem->setItemName(m_oldTrackItem.itemName());
    m_targetItem->setWidth(m_oldTrackItem.width());
    m_targetItem->setType(m_oldTrackItem.trackType());
    // update widget values if there is one
    if ( m_targetItem->tool() )
         m_targetItem->tool()->updateTool();
    qApp->selectionTree()->updateName(m_targetItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_targetItem->groupBoundingRect(),m_targetItem->layer());
}

void EditTrackUndoCommand::redo(){
    m_targetItem->setItemName(m_newTrackItem->itemName());
    m_targetItem->setWidth(m_newTrackItem->width());
    m_targetItem->setType(m_newTrackItem->trackType());
    // update widget values if there is one
    if ( m_targetItem->tool() )
         m_targetItem->tool()->updateTool();
    qApp->selectionTree()->updateName(m_targetItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_targetItem->groupBoundingRect(),m_targetItem->layer());
}
