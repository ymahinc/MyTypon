#include "splittrackundocommand.h"
#include "global.h"

SplitTrackUndoCommand::SplitTrackUndoCommand(Angle *angle){
    setText(QObject::tr("Split"));
    // keep trace of angle where track is splitted
    m_angle = angle; 
    // clone the original track into the newly created one to get all its properties
    m_track = dynamic_cast<Track *>(m_angle->parentItem());
    m_newTrack = m_track->clone();
    // clear newly created track of all its angles
    for ( int i = m_newTrack->angles().count()-1; i >= 0; i--)
        m_newTrack->removeAngle(m_newTrack->angles().at(i),true);
    // create a new angle that is the new fist angle of the newly created track
    m_newFirstAngle = new Angle(m_newTrack);
}

SplitTrackUndoCommand::~SplitTrackUndoCommand(){
    if ( ! m_newTrack->parentItem() )
        delete m_newTrack;
}

void SplitTrackUndoCommand::undo(){
    // add all angles but first one of new track to the source track
    for(int i = 1; i < m_newTrack->angles().count(); i++)
        m_track->appendAngle(m_newTrack->angles().at(i));
    // remove newly created track from scene
    qApp->currentTypon()->typonScene()->removeTyponItem(m_newTrack);
    // clear all angles in new track
    for ( int i = m_newTrack->angles().count()-1; i >= 0; i--)
        m_newTrack->removeAngle(m_newTrack->angles().at(i),false);
    // update source track
    m_track->onTyponSelectedChange();
}

void SplitTrackUndoCommand::redo(){  
    // get splitting angle index and angle count
    int angleIndex = m_track->angles().indexOf(m_angle);
    int angleCount = m_track->angles().count();

    // set the position of the new fist angle of the new track at the splitting angle position
    // add this new angle to the new track
    m_newFirstAngle->setGroupPos(m_track->angles().at(angleIndex)->groupPos());
    m_newTrack->appendAngle(m_newFirstAngle);

    // fill newly created track with all following angles of source track
    for(int i = angleIndex+1; i < angleCount ; i++)
        m_newTrack->appendAngle(m_track->angles().at(i));

    // remove all angles that were added to new track from source one
    for(int i = angleCount-1; i > angleIndex ; i--)
        m_track->removeAngle(m_track->angles().at(i),false);

    // update source track
    m_track->updateGeometry();

    // update new track layer and selection
    m_newTrack->setParentItem(m_track->layer());
    if ( m_track->typonItemIsSelected() )
        qApp->currentTypon()->typonScene()->addSelectedItem(m_newTrack);
    else
        qApp->currentTypon()->typonScene()->removeSelectedItem(m_newTrack);
}
