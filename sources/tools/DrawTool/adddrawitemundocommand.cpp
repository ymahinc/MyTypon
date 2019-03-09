#include "adddrawitemundocommand.h"
#include "global.h"
#include <QDebug>

AddDrawItemUndoCommand::AddDrawItemUndoCommand(DrawItem *item){
    m_layer = item->layer();
    m_drawItem = item->clone();

    if ( item->drawType() == DrawItem::Rectangle ){
        setText(QObject::tr("Add Rectangle"));
        m_drawItem->setItemName(QObject::tr("Rectangle %1").arg(QString::number(++qApp->currentTypon()->rectCounter)));
    }
    if ( item->drawType() == DrawItem::Polygon ){
        setText(QObject::tr("Add Polygon"));
        m_drawItem->setItemName(QObject::tr("Polygon %1").arg(QString::number(++qApp->currentTypon()->polygonCounter)));
    }
    if ( item->drawType() == DrawItem::Ellipse ){
        setText(QObject::tr("Add Ellipse"));
        m_drawItem->setItemName(QObject::tr("Ellipse %1").arg(QString::number(++qApp->currentTypon()->ellipseCounter)));
    }
}

AddDrawItemUndoCommand::AddDrawItemUndoCommand(QDataStream &stream, MYItemBase *group){
    m_drawItem = new DrawItem(0);
    int layerZValue = m_drawItem->restore(stream);
    m_layer = qApp->currentTypon()->typonScene()->layers()->layerFromZValue(layerZValue);
    if ( group )
        group->addToGroup(m_drawItem);

    if ( m_drawItem->drawType() == DrawItem::Rectangle )
        setText(QObject::tr("Add Rectangle"));

    if ( m_drawItem->drawType() == DrawItem::Polygon )
        setText(QObject::tr("Add Polygon"));

    if ( m_drawItem->drawType() == DrawItem::Ellipse )
        setText(QObject::tr("Add Ellipse"));
}

AddDrawItemUndoCommand::~AddDrawItemUndoCommand(){
    if ( ! m_drawItem->parentItem() )
        delete m_drawItem;
}

void AddDrawItemUndoCommand::undo(){
    qApp->currentTypon()->typonScene()->removeTyponItem(m_drawItem);
    m_layer->groundPlanedItems()->removeOne(m_drawItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_drawItem->boundingRect(),m_layer);
}

void AddDrawItemUndoCommand::redo(){
    m_drawItem->setParentItem(m_layer);
    if ( m_drawItem->isGroundPlane() )
        m_layer->groundPlanedItems()->append(m_drawItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_drawItem->boundingRect(),m_layer);
}
