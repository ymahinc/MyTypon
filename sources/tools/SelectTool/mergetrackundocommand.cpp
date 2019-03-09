#include "mergetrackundocommand.h"
#include "global.h"
#include <QDebug>

MergeTrackUndoCommand::MergeTrackUndoCommand(Angle *angle1, Angle *angle2){
    setText(QObject::tr("Merge"));
    // keep pointer to the two merging angles
    m_angle1 = angle1;
    m_angle2 = angle2;
    // keep index of merging angle in order to split at the right angle when undoing
    mergeAngleIndex = 0;
    // get parent tracks for given angles
    m_track1 = dynamic_cast<Track *>(m_angle1->parentItem());
    m_track2 = dynamic_cast<Track *>(m_angle2->parentItem());
    // hold the type of merging (by first points, last points, or last and first)
    m_mergeSense = 1;
}

void MergeTrackUndoCommand::undo(){
    if ( ! m_track1 )
        return;
    if ( ! m_track2 )
        return;

    // tracks were merged via first angle of first track and last angle of second track
    if ( m_mergeSense == 0 ) {
        // refill previously merged track with corresponding angles
        for(int i = mergeAngleIndex+1; i < m_track2->angles().count(); i++)
            m_track1->appendAngle(m_track2->angles().at(i),false);

        // remove angles added in first track when merging
        for(int i = m_track2->angles().count()-1; i > mergeAngleIndex ; i--)
            m_track2->removeAngle(m_track2->angles().at(i),false);

        // update tracks
        m_track1->updateGeometry();
        m_track2->updateGeometry();

        // add merged track to scene again and update selection accordingly
        m_track1->setParentItem(m_track2->layer());
        if ( m_track2->typonItemIsSelected() )
            qApp->currentTypon()->typonScene()->addSelectedItem(m_track1);
        else
            qApp->currentTypon()->typonScene()->removeSelectedItem(m_track1);
    }

    if ( m_mergeSense == 1 ) {
        // refill previously merged track with corresponding angles
        for(int i = mergeAngleIndex+1; i < m_track1->angles().count(); i++)
            m_track2->appendAngle(m_track1->angles().at(i),false);

        // remove angles added in first track when merging
        for(int i = m_track1->angles().count()-1; i > mergeAngleIndex ; i--)
            m_track1->removeAngle(m_track1->angles().at(i),false);

        // update tracks
        m_track1->updateGeometry();
        m_track2->updateGeometry();

        // add merged track to scene again and update selection accordingly
        m_track2->setParentItem(m_track1->layer());
        if ( m_track1->typonItemIsSelected() )
            qApp->currentTypon()->typonScene()->addSelectedItem(m_track2);
        else
            qApp->currentTypon()->typonScene()->removeSelectedItem(m_track2);
    }

    // tracks were merged via last angles
    if ( m_mergeSense == 2 ){
        // refill previously merged track with corresponding angles
        for(int i = mergeAngleIndex+1; i < m_track1->angles().count(); i++)
            m_track2->prependAngle(m_track1->angles().at(i),false);

        // remove angles added in first track when merging
        for(int i = m_track1->angles().count()-1; i > mergeAngleIndex ; i--)
            m_track1->removeAngle(m_track1->angles().at(i),false);

        // update tracks
        m_track1->updateGeometry();
        m_track2->updateGeometry();

        // add merged track to scene again and update selection accordingly
        m_track2->setParentItem(m_track1->layer());
        if ( m_track1->typonItemIsSelected() )
            qApp->currentTypon()->typonScene()->addSelectedItem(m_track2);
        else
            qApp->currentTypon()->typonScene()->removeSelectedItem(m_track2);
    }

    // tracks were merged via first angles
    if ( m_mergeSense == 3 ){
        // refill previously merged track with corresponding angles
        for(int i = mergeAngleIndex-1; i >= 0; i--)
            m_track2->appendAngle(m_track1->angles().at(i),false);

        // remove angles added in first track when merging
        for(int i = 0; i < mergeAngleIndex; i++)
            m_track1->removeAngle(m_track1->angles().at(0),false);

        // update tracks
        m_track1->updateGeometry();
        m_track2->updateGeometry();

        // add merged track to scene again and update selection accordingly
        m_track2->setParentItem(m_track1->layer());
        if ( m_track1->typonItemIsSelected() )
            qApp->currentTypon()->typonScene()->addSelectedItem(m_track2);
        else
            qApp->currentTypon()->typonScene()->removeSelectedItem(m_track2);
    }

}

void MergeTrackUndoCommand::redo(){
    if ( ! m_track1 )
        return;
    if ( ! m_track2 )
        return;

    // Merge tracks with fist angle of first track and last angle of last track
    if ( m_track2->isLastAngle(m_angle2) && m_track1->isFirstAngle(m_angle1) ){
        // get merging index
        mergeAngleIndex = m_track2->angles().count() - 1;
        // append all angles except first one of the second tracks into the first track
        for(int i = 1; i < m_track1->angles().count(); i++)
            m_track2->appendAngle(m_track1->angles().at(i));
        // clear all angles of second track execpt first one
        for(int i = m_track1->angles().count()-1; i > 0 ; i--)
            m_track1->removeAngle(m_track1->angles().at(i),false);
        // remove the merged track from the scene
        qApp->currentTypon()->typonScene()->removeTyponItem(m_track1);
        // update the modified track
        m_track2->onTyponSelectedChange();
        // set the merging mode flag
        m_mergeSense = 0;
    }
    // Merge tracks with last angle of first track and first angle of last track
    if ( m_track2->isFirstAngle(m_angle2) && m_track1->isLastAngle(m_angle1) ){
        // get merging index
        mergeAngleIndex = m_track1->angles().count() - 1;
        // append all angles except first one of the second tracks into the first track
        for(int i = 1; i < m_track2->angles().count(); i++)
            m_track1->appendAngle(m_track2->angles().at(i));
        // clear all angles of second track execpt first one
        for(int i = m_track2->angles().count()-1; i > 0 ; i--)
            m_track2->removeAngle(m_track2->angles().at(i),false);
        // remove the merged track from the scene
        qApp->currentTypon()->typonScene()->removeTyponItem(m_track2);
        // update the modified track
        m_track1->onTyponSelectedChange();
        // set the merging mode flag
        m_mergeSense = 1;
    }
    // Merge tracks via their last angles
    if ( m_track2->isLastAngle(m_angle2) && m_track1->isLastAngle(m_angle1) ){
        // get merging index
        mergeAngleIndex = m_track1->angles().count() - 1;
        // add all angles except last one of the second tracks into the first track
        // get angles to add reversly in order to keep angle order safe
        for(int i = m_track2->angles().count() - 2; i >= 0; i-- )
            m_track1->appendAngle(m_track2->angles().at(i));
        // remove all angles except last one in merged track
        for(int i = 0; i < m_track2->angles().count()-1 ; i++)
            m_track2->removeAngle(m_track2->angles().at(i),false);
        // remove merged track from scene
        qApp->currentTypon()->typonScene()->removeTyponItem(m_track2);
        // update modified track
        m_track1->onTyponSelectedChange();
        // set the merging mode flag
        m_mergeSense = 2;
    }
    // We merge tracks by their first point
    if ( m_track2->isFirstAngle(m_angle2) && m_track1->isFirstAngle(m_angle1) ){
        // get merging index
        mergeAngleIndex = m_track2->angles().count() - 1;
        // prepend all angles except first one of the second track into the first one
        for(int i = 1; i < m_track2->angles().count(); i++)
            m_track1->prependAngle(m_track2->angles().at(i));
        // remove all angles except first one in merged track
        for(int i = m_track2->angles().count()-1; i > 0  ; i--)
            m_track2->removeAngle(m_track2->angles().at(i),false);
        // remove merged track from scene
        qApp->currentTypon()->typonScene()->removeTyponItem(m_track2);
        // update modified track
        m_track1->onTyponSelectedChange();
        // set the merging mode flag
        m_mergeSense = 3;
    }
}
