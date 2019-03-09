#include "drawitem.h"
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include "global.h"
#include <QBitmap>
#include "items/trackitem/angle.h"

DrawItem::DrawItem(MYItemBase *parent)
    : Track(parent){
    m_tool = 0;
    m_fillType = NoFill;
    m_drawType = Rectangle;
}

void DrawItem::save(QDataStream &stream){
    stream << 4 << m_name << pos() << transform() << rotation() << layer()->realLayerZValue()
    << (int)m_drawType << m_width << (int) m_fillType
    << m_angles.count();

    foreach (Angle *angle, m_angles)
        stream << angle->scenePos();
}

int DrawItem::restore(QDataStream &stream){
    QString name;
    QPointF pos;
    QTransform transform;
    double rotation;
    int layerZValue;
    int drawType;
    double width;
    int fillType;

    stream >> name >> pos >> transform >> rotation >> layerZValue >> drawType >> width >> fillType;
    setItemName(name);
    setPos(pos);
    setRotation(rotation);
    setTransform(transform,true);

    setDrawType((DrawType)drawType);
    setWidth(width);

    int anglesCount;
    stream >> anglesCount;

    for(int i = 0; i < anglesCount; i++){
        QPointF anglePos;
        stream >> anglePos;
        appendAngle(anglePos,false);
    }

    setFillType((FillType)fillType);

    return layerZValue;
}

DrawItem *DrawItem::clone(){
    DrawItem *clone = new DrawItem(0);
    clone->prepareGeometryChange();
    clone->setItemName(this->itemName());
    clone->setWidth(m_width);
    clone->setFillType(m_fillType);
    clone->setDrawType(m_drawType);
    clone->setGroundPlaneOffset(this->groundPlaneOffset());
    clone->setGroundPlane(this->isGroundPlane());

    foreach (Angle *angle, m_angles)
        clone->appendAngle(angle->scenePos(),false);

    return clone;
}

DrawItem::DrawType DrawItem::drawType(){
    return m_drawType;
}

void DrawItem::setDrawType(DrawType drawType){
    m_drawType = drawType;
    updateGeometry();
}

DrawItem::FillType DrawItem::fillType(){
    return m_fillType;
}

void DrawItem::setFillType(FillType type){
    m_fillType = type;
    setGroundPlane( m_fillType == GroundPlane );
    updateGeometry();
}

void DrawItem::setTool(DrawTool *tool){
    m_tool = tool;
}

DrawTool *DrawItem::tool(){
    return m_tool;
}

QPainterPath DrawItem::shape() const{
    return m_shape;
}

QRectF DrawItem::boundingRect() const{
    if ( ! isVisible() )
        return QRectF();
    else
        return m_bRect;
}

void DrawItem::removeAngle(Angle *angle, bool deleteAfterRemove){
    Track::removeAngle(angle,deleteAfterRemove);
    updateGeometry();
}

void DrawItem::updateAnglePos(Angle *angle, QPointF pos){
    if ( m_drawType == Rectangle || m_drawType == Ellipse ){
        QPointF newPos = mapFromParent(pos);
        if ( angle == m_angles.at(0) ){
            angle->setPos(newPos);
            m_angles.at(1)->setPos(QPointF(m_angles.at(1)->pos().x(),newPos.y()));
            m_angles.at(3)->setPos(QPointF(newPos.x(),m_angles.at(3)->pos().y()));
        }
        if ( angle == m_angles.at(1) ){
            angle->setPos(newPos);
            m_angles.at(0)->setPos(QPointF(m_angles.at(0)->pos().x(),newPos.y()));
            m_angles.at(2)->setPos(QPointF(newPos.x(),m_angles.at(2)->pos().y()));
        }
        if ( angle == m_angles.at(2) ){
            angle->setPos(newPos);
            m_angles.at(1)->setPos(QPointF(newPos.x(),m_angles.at(0)->pos().y()));
            m_angles.at(3)->setPos(QPointF(m_angles.at(0)->pos().x(),newPos.y()));
        }
        if ( angle == m_angles.at(3) ){
            angle->setPos(newPos);
            m_angles.at(0)->setPos(QPointF(newPos.x(),m_angles.at(0)->pos().y()));
            m_angles.at(2)->setPos(QPointF(m_angles.at(2)->pos().x(),newPos.y()));
        }
    }
    if ( m_drawType == Polygon )
        Track::updateAnglePos(angle,pos);

    updateGeometry();
}

void DrawItem::appendAngle(QPointF pos, bool checkLastPos){
    Track::appendAngle(pos,checkLastPos);
    updateGeometry();
}

void DrawItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget){
    Q_UNUSED(widget);
    if ( ! layer() )
        return;

    QColor color;
    if ( isToolItem() )
        color = layer()->color().lighter();
    else
        color = layer()->color();

    if ( m_isSelected )
        color = Qt::white;

    if ( m_fillType != GroundPlane ){
        painter->setPen(Qt::NoPen);
        painter->setClipRect(option->exposedRect);
        painter->fillPath(m_shape,QBrush(color));
    }

    if ( m_fillType == GroundPlane ){
        painter->setOpacity(0.7);
        painter->setPen(Qt::NoPen);
        painter->setClipPath(m_shape);
        painter->fillPath(m_shape,QBrush(color));
        for ( int i = 0; i < m_paths.count(); i ++)
            painter->fillPath(m_paths.at(i),qApp->workSpace()->grid->backgroundColor());
    }
}

void DrawItem::updateGeometry(){
    prepareGeometryChange();

    if ( m_angles.count() < 2 )
        return;

    if ( m_drawType == Ellipse && m_angles.count() < 4 )
        return;

    if ( m_drawType == Rectangle && m_angles.count() < 4 )
        return;

    m_paths.clear();
    m_shape = QPainterPath();
    m_bRect = QRectF();

    QPainterPathStroker ps;
    ps.setCapStyle(Qt::RoundCap);
    ps.setWidth(m_width);
    ps.setJoinStyle(Qt::RoundJoin);
    ps.setMiterLimit(0);

    QPainterPath path;

    if ( m_drawType == Ellipse ){
        path.addEllipse(QRectF(m_angles.at(0)->pos(),m_angles.at(2)->pos()).normalized());
    }
    if ( m_drawType == Rectangle ){
        path.addRect(QRectF(m_angles.at(0)->pos(),m_angles.at(2)->pos()).normalized());
    }
    if ( m_drawType == Polygon ){
        QPolygonF p;
        for ( int i = 0; i < m_angles.count(); i++ )
            p.append(m_angles.at(i)->pos());

        path.addPolygon(p);
        path.closeSubpath();
    }

    m_shape = ps.createStroke(path);
    m_bRect = m_shape.boundingRect();

    if ( m_fillType == FullFill || m_fillType == GroundPlane ){
        if ( m_drawType == Polygon ){
            m_shape = m_shape.united(path);
        }else{
            m_shape.addPath(path);
        }
    }

    if ( m_fillType == GroundPlane )
        m_paths = collidingPaths();
}
