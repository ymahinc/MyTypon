#include "holeitem.h"
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include "global.h"

HoleItem::HoleItem(MYItemBase *parent)
    : MYItemBase(parent){
    setFlag(QGraphicsItem::ItemIsSelectable,false);
    setFlag(QGraphicsItem::ItemIsMovable,false);
    m_width = 25;
    m_bRect = QRectF();
    m_shape = QPainterPath();
    setItemName(QObject::tr("Hole"));
    m_originalZValue = 6;
    setZValue(6);
    updateGeometry();
}

QPainterPath HoleItem::shape() const{
    return m_shape;
}

//return correct bounding rect for display update
QRectF HoleItem::boundingRect() const {
    if ( ! isVisible() )
        return QRectF();
    else
        return m_bRect;
}

// clone the line, used to add it to the scene from tracktool
HoleItem *HoleItem::clone(){
    HoleItem *clone = new HoleItem(0);
    clone->prepareGeometryChange();
    clone->setWidth(m_width);
    clone->setItemName(m_name);
    return clone;
}

void HoleItem::setWidth(double width){
    m_width = width;
    updateGeometry();
}

double HoleItem::width(){
    return m_width;
}

void HoleItem::updateGeometry(){
    prepareGeometryChange();

    double halhWidth = m_width /2.0;
    double sixthWidth = m_width /6.0;

    m_shape = QPainterPath();
    m_path = QPainterPath();

    m_bRect = QRectF(-halhWidth,-halhWidth,m_width,m_width);
    m_shape.addEllipse(m_bRect);
    m_path.addEllipse(m_bRect);
    m_path.moveTo(halhWidth+sixthWidth,0);
    m_path.lineTo(-halhWidth-sixthWidth,0);
    m_path.moveTo(0,-halhWidth-sixthWidth);
    m_path.lineTo(0,halhWidth+sixthWidth);
    m_bRect = m_path.boundingRect();
}

void HoleItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget){
    Q_UNUSED(widget);
    if ( ! layer() )
        return;
    painter->setClipRect( option->exposedRect );

    QColor color;
    if ( isToolItem() )
        color = layer()->color().lighter();
    else
        color = layer()->color();

    if ( typonItemIsSelected() )
        color = Qt::white;

    painter->setPen(Qt::NoPen);
    QRectF holeBoundincRect(-m_width/2.0,-m_width/2.0,m_width,m_width);
    QPainterPath holePath;
    holePath.addEllipse(holeBoundincRect);
    painter->fillPath(holePath,qApp->workSpace()->grid->backgroundColor());

    QPen pen;
    pen.setColor(color);
    pen.setWidthF(0);
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->drawPath(m_path);
}
