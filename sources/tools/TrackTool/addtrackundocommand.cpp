#include "addtrackundocommand.h"
#include "global.h"
#include <QDebug>

AddTrackUndoCommand::AddTrackUndoCommand(Track *track){
    setText(QObject::tr("Add track"));
    m_layer = track->layer();
    m_track = track->clone();
}

AddTrackUndoCommand::AddTrackUndoCommand(QDataStream &stream, MYItemBase *group){
    setText(QObject::tr("Add track"));
    m_track = new Track(0);
    int layerZValue = m_track->restore(stream);
    m_layer = qApp->currentTypon()->typonScene()->layers()->layerFromZValue(layerZValue);
    if ( group )
        group->addToGroup(m_track);
}

AddTrackUndoCommand::~AddTrackUndoCommand(){
    if ( ! m_track->parentItem() )
        delete m_track;
}

void AddTrackUndoCommand::undo(){
    qApp->currentTypon()->typonScene()->removeTyponItem(m_track);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_track->groupBoundingRect(),m_layer);
}

void AddTrackUndoCommand::redo(){
    m_track->setParentItem(m_layer);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_track->groupBoundingRect(),m_layer);
}
