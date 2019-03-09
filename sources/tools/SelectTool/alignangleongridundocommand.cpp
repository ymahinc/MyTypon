#include "alignangleongridundocommand.h"
#include "global.h"

AlignAngleOnGridUndoCommand::AlignAngleOnGridUndoCommand(Angle *angle, QPointF newPos){
    setText(QObject::tr("Align Angle"));
    m_angle = angle;
    m_oldPos = angle->groupPos();
    m_newPos = newPos;
}

void AlignAngleOnGridUndoCommand::undo(){
    Track *track = dynamic_cast<Track *>(m_angle->parentItem());
    if ( !track )
        return;
    track->updateAnglePos(m_angle, m_oldPos);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(track->groupBoundingRect());
}

void AlignAngleOnGridUndoCommand::redo(){
    Track *track = dynamic_cast<Track *>(m_angle->parentItem());
    if ( !track )
        return;
    track->updateAnglePos(m_angle,
                          qApp->currentTypon()->typonView()->nearestGridPoint(m_newPos));
    qApp->currentTypon()->typonScene()->updateGroundPlanes(track->groupBoundingRect());
}
