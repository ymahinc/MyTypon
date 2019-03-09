#include "changelayerundocommand.h"
#include "global.h"

ChangeLayerUndoCommand::ChangeLayerUndoCommand(MYItemBase *item, Layer *layer){
    setText(QObject::tr("Change layer"));
    // find if item is a pad item
    m_pad = 0;
    m_pad = dynamic_cast<PadGroup *>(item);
    m_item = item;
    m_layer = layer;
    // get old layer, if item is a pad get pad layer
    if ( m_pad )
        m_oldLayer = m_pad->pad()->layer();
    else
        m_oldLayer = item->layer();
}

void ChangeLayerUndoCommand::undo(){
    // if item is a pad it has its own setLayer function as it is a particular case of group
    if ( m_pad )
        m_pad->setLayer(m_oldLayer);
    else
        m_item->setLayer(m_oldLayer);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_item->groupBoundingRect());
}

void ChangeLayerUndoCommand::redo(){
    // if item is a pad it has its own setLayer function as it is a particular case of group
    if ( m_pad )
        m_pad->setLayer(m_layer);
    else
        m_item->setLayer(m_layer);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_item->groupBoundingRect());
}
