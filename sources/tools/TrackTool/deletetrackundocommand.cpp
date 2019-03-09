#include "deletetrackundocommand.h"
#include "global.h"
#include <QDebug>

DeleteTrackUndoCommand::DeleteTrackUndoCommand(Track *item){
    setText(QObject::tr("Delete Track"));
    m_trackItem = item;
    m_layer = item->layer();
    m_pos = item->pos();
}

void DeleteTrackUndoCommand::undo(){
    m_trackItem->setParentItem(m_layer);
    m_trackItem->setPos(m_pos);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_trackItem->groupBoundingRect(),m_layer);
}

void DeleteTrackUndoCommand::redo(){
    qApp->currentTypon()->typonScene()->removeTyponItem(m_trackItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_trackItem->groupBoundingRect(),m_layer);
}
