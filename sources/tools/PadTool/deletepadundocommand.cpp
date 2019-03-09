#include "deletepadundocommand.h"
#include "global.h"
#include <QDebug>

DeletePadUndoCommand::DeletePadUndoCommand(PadGroup *pad){
    setText(QObject::tr("Delete Pad"));
    m_pad = pad;
    m_layer = m_pad->pad()->layer();
    m_pos = m_pad->groupPos();
}

void DeletePadUndoCommand::undo(){
    m_pad->setLayer(m_layer);
    m_pad->setGroupPos(m_pos);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_pad->groupBoundingRect());
}

void DeletePadUndoCommand::redo(){
    m_pad->setLayer(0);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_pad->groupBoundingRect());
}

