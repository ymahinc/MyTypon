#include "removeangleundocommand.h"
#include "items/trackitem/angle.h"
#include "global.h"

RemoveAngleUndoCommand::RemoveAngleUndoCommand(Angle *angle){
    setText(QObject::tr("Remove Angle"));
    m_angle = angle;
    m_pos = angle->pos();
    m_track = dynamic_cast<Track *>(angle->parentItem());
    m_index = 0;
    if ( m_track )
        m_index = m_track->angles().indexOf(angle);
}

RemoveAngleUndoCommand::~RemoveAngleUndoCommand(){

}

void RemoveAngleUndoCommand::undo(){
    if ( m_track ){
        m_track->insertAngle(m_angle,m_index,m_pos);
        qApp->currentTypon()->typonScene()->updateGroundPlanes(m_track->groupBoundingRect());
    }
}

void RemoveAngleUndoCommand::redo(){
    if ( m_track ){
        m_track->removeAngle(m_angle,false);
        qApp->currentTypon()->typonScene()->updateGroundPlanes(m_track->groupBoundingRect());
    }
}
