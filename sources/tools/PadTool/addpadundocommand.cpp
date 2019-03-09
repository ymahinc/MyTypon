#include "addpadundocommand.h"
#include "global.h"
#include <QDebug>

AddPadUndoCommand::AddPadUndoCommand(PadGroup *item, QPointF pos, Layer *layer){
    setText(QObject::tr("Add Pad"));
    m_layer = layer;
    m_pad = item->clone();
    m_pos = pos;
}

AddPadUndoCommand::AddPadUndoCommand(QDataStream &stream, MYItemBase *group){
    setText(QObject::tr("Add Pad"));
    m_pad = new PadGroup(0);
    int layerZValue = m_pad->restore(stream);
    m_layer = qApp->currentTypon()->typonScene()->layers()->layerFromZValue(layerZValue);
    m_pos = m_pad->groupPos();
    if ( group )
        group->addToGroup(m_pad);
}

AddPadUndoCommand::~AddPadUndoCommand(){
    m_pad->removeFromGroup(m_pad->pad());
    m_pad->removeFromGroup(m_pad->hole());
    if ( ! m_pad->pad()->parentItem() )
        delete m_pad->pad();
    if ( ! m_pad->hole()->parentItem() )
        delete m_pad->hole();
    delete m_pad;
}

void AddPadUndoCommand::undo(){
    m_pad->setLayer(0);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_pad->groupBoundingRect());
}

void AddPadUndoCommand::redo(){
    m_pad->setLayer(m_layer);
    m_pad->setGroupPos(m_pos);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_pad->groupBoundingRect());
}
