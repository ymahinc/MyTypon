#include "angle.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

Angle::Angle(QGraphicsItem * parent)
    : MYItemBase(parent){
    m_width = 10;
    m_isAngle = true;
}

void Angle::save(QDataStream &stream){
    Q_UNUSED(stream)
}

int Angle::restore(QDataStream &stream){
    Q_UNUSED(stream)
}

void Angle::setWidth(double width){
    m_width = width;
    updateGeometry();
}

double Angle::width(){
    return m_width;
}

QRectF Angle::boundingRect() const{
    if ( ! isVisible() )
        return QRectF();
    else
        return m_bRect;
}

QPainterPath Angle::shape() const{
    return m_shape;
}

void Angle::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget){
    Q_UNUSED(widget);
    Q_UNUSED(option)
    painter->setClipRect( option->exposedRect );
    painter->setPen(Qt::NoPen);
    painter->fillPath(m_shape,Qt::yellow);
}

void Angle::updateGeometry(){
    prepareGeometryChange();

    m_shape = QPainterPath();
    m_shape.addEllipse(-m_width/2.0,-m_width/2.0,m_width,m_width);
    m_bRect = m_shape.boundingRect();
}
