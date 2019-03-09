#include "alignongridundocommand.h"
#include "global.h"

AlignOnGridUndoCommand::AlignOnGridUndoCommand(MYItemBase *item){
    setText(QObject::tr("Align"));
    m_item = item;
    m_oldPos = item->groupPos();
}

void AlignOnGridUndoCommand::undo(){
    m_item->setGroupPos(m_oldPos);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_item->groupBoundingRect());
}

void AlignOnGridUndoCommand::redo(){
    m_item->setGroupPos(qApp->currentTypon()->typonView()->nearestGridPoint(m_item->groupPos()));
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_item->groupBoundingRect());
}
