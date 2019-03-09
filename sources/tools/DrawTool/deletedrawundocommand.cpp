#include "deletedrawundocommand.h"
#include "global.h"
#include <QDebug>

DeleteDrawUndoCommand::DeleteDrawUndoCommand(DrawItem *item){
    if ( item->drawType() == DrawItem::Rectangle )
        setText(QObject::tr("Delete Rectangle"));
    if ( item->drawType() == DrawItem::Polygon )
        setText(QObject::tr("Delete Polygon"));
    if ( item->drawType() == DrawItem::Ellipse )
        setText(QObject::tr("Delete Ellipse"));
    m_drawItem = item;
    m_layer = item->layer();
    m_pos = item->pos();
}

void DeleteDrawUndoCommand::undo(){
    m_drawItem->setParentItem(m_layer);
    m_drawItem->setPos(m_pos);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_drawItem->groupBoundingRect(),m_layer);
}

void DeleteDrawUndoCommand::redo(){
    qApp->currentTypon()->typonScene()->removeTyponItem(m_drawItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_drawItem->groupBoundingRect(),m_layer);
}
