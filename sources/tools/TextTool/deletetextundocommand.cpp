#include "deletetextundocommand.h"
#include "global.h"
#include <QDebug>

DeleteTextUndoCommand::DeleteTextUndoCommand(TextItem *item){
    setText(QObject::tr("Delete Text"));
    m_textItem = item;
    m_layer = item->layer();
    m_pos = item->pos();
}

void DeleteTextUndoCommand::undo(){
    m_textItem->setParentItem(m_layer);
    m_textItem->setPos(m_pos);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_textItem->groupBoundingRect(),m_layer);
}

void DeleteTextUndoCommand::redo(){
    qApp->currentTypon()->typonScene()->removeTyponItem(m_textItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_textItem->groupBoundingRect(),m_layer);
}
