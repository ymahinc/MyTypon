#include "moveangleundocommand.h"
#include "global.h"

MoveAngleUndoCommand::MoveAngleUndoCommand(Angle *angle, QPointF oldPos, QPointF pos){
    setText(QObject::tr("Move Angle"));
    m_angle = angle;
    m_pos = pos;
    m_oldPos = oldPos;
    m_firstRedo = true;
}

void MoveAngleUndoCommand::undo(){
    Track *track = dynamic_cast<Track *>(m_angle->parentItem());
    if ( !track )
        return;

    track->updateAnglePos(m_angle,m_oldPos);

    qApp->currentTypon()->typonScene()->updateGroundPlanes(track->groupBoundingRect());
}

void MoveAngleUndoCommand::redo(){
    Track *track = dynamic_cast<Track *>(m_angle->parentItem());
    if ( !track )
        return;

    if ( !m_firstRedo ) // on fist redo angle is already at the desired position, do nothing
        track->updateAnglePos(m_angle,m_pos);
    else
        m_firstRedo = false;

    qApp->currentTypon()->typonScene()->updateGroundPlanes(track->groupBoundingRect());
}
