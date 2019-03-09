#include "insertangleundocommand.h"
#include "global.h"

InsertAngleUndoCommand::InsertAngleUndoCommand(Track *track, QPointF pos){
    setText(QObject::tr("Insert Angle"));
    m_track = track;
    m_angle = new Angle(track);
    angleIsOwnedByTrack = false;
    m_index = track->indexOfPoint(pos);
    // get nearest line pos for the given angle
    m_pos = track->linePoint(pos,m_index);
}

InsertAngleUndoCommand::~InsertAngleUndoCommand(){
    if ( ! angleIsOwnedByTrack )
        delete m_angle;
}

void InsertAngleUndoCommand::undo(){
    m_track->removeAngle(m_angle,false);
    angleIsOwnedByTrack = false;
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_track->groupBoundingRect());
}

void InsertAngleUndoCommand::redo(){
    m_track->insertAngle(m_angle,m_index,m_pos);
    angleIsOwnedByTrack = true;
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_track->groupBoundingRect());
}

