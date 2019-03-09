#include "paditem.h"
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include "global.h"

PadItem::PadItem(MYItemBase *parent)
    : MYItemBase(parent){
    m_hole = 0;
    m_type = Round;
    setFlag(QGraphicsItem::ItemIsSelectable,false);
    setFlag(QGraphicsItem::ItemIsMovable,false);
    m_width = 63;
    m_bRect = QRectF();
    m_shape = QPainterPath();
    m_path = QPainterPath();
    setItemName(QObject::tr("PadIt"));
    updateGeometry();
    m_originalZValue = 4;
    setZValue(4);
}

PadItem::~PadItem(){
}

void PadItem::setType(PadType type){
    updateGeometry();
    m_type = type;
    updateGeometry();
}

PadItem::PadType PadItem::padType(){
    return m_type;
}

QPainterPath PadItem::shape() const{
    return m_shape;
}

//return correct bounding rect for display update
QRectF PadItem::boundingRect() const {
    if ( ! isVisible() )
        return QRectF();
    else
        return m_bRect;
}

// clone the line, used to add it to the scene from tracktool
PadItem *PadItem::clone(){
    PadItem *clone = new PadItem(0);
    clone->prepareGeometryChange();
    clone->setType(m_type);
    clone->setWidth(m_width);
    clone->setItemName(m_name);
    return clone;
}

//pointer to pad hole in order to update this drawing even if drill layer is hidden
HoleItem *PadItem::hole(){
    return m_hole;
}

void PadItem::setWidth(double width){
    m_width = width;
    updateGeometry();
}

double PadItem::width(){
    return m_width;
}

void PadItem::updateGeometry(){
    prepareGeometryChange();

    double halhWidth = m_width /2.0;
    double sixthWidth = m_width /6.0;

    m_shape = QPainterPath();

    if (m_type == Round){
        m_bRect = QRectF(-halhWidth,-halhWidth,m_width,m_width);
        m_shape.addEllipse(m_bRect);
    }

    if (m_type == Square){
        m_bRect = QRectF(-halhWidth,-halhWidth,m_width,m_width);
        m_shape.addRect(m_bRect);
    }

    if (m_type == PolySquare){
        m_shape.moveTo(halhWidth,sixthWidth);
        m_shape.lineTo(halhWidth,-sixthWidth);
        m_shape.lineTo(sixthWidth,-halhWidth);
        m_shape.lineTo(-sixthWidth,-halhWidth);
        m_shape.lineTo(-halhWidth,-sixthWidth);
        m_shape.lineTo(-halhWidth,sixthWidth);
        m_shape.lineTo(-sixthWidth,halhWidth);
        m_shape.lineTo(sixthWidth,halhWidth);
        m_shape.closeSubpath();
        m_bRect = m_shape.boundingRect();
    }

    if (m_type == RoundSquare){
        m_bRect = QRectF(-halhWidth,-halhWidth,m_width,m_width);
        m_shape.addRoundRect(m_bRect,25);
    }

    if (m_type == Ellipse){
        m_bRect = QRectF(-halhWidth,-halhWidth/2.0,m_width,m_width/2.0);
        m_shape.addEllipse(m_bRect);
    }

    if (m_type == Rectangle){
        m_bRect = QRectF(-halhWidth,-halhWidth/2.0,m_width,m_width/2.0);
        m_shape.addRect(m_bRect);
    }

    if (m_type == Polygon){
        m_shape.moveTo(halhWidth,sixthWidth/2.0);
        m_shape.lineTo(halhWidth,-sixthWidth/2.0);
        m_shape.lineTo(halhWidth - sixthWidth,-halhWidth/2.0);
        m_shape.lineTo(-halhWidth+sixthWidth,-halhWidth/2.0);
        m_shape.lineTo(-halhWidth,-sixthWidth/2.0);
        m_shape.lineTo(-halhWidth,sixthWidth/2.0);
        m_shape.lineTo(-halhWidth+sixthWidth,halhWidth/2.0);
        m_shape.lineTo(halhWidth-sixthWidth,halhWidth/2.0);
        m_shape.closeSubpath();
        m_bRect = m_shape.boundingRect();
    }

    if (m_type == RoundRectangle){
        m_bRect = QRectF(-halhWidth,-halhWidth/2.0,m_width,m_width/2.0);
        m_shape.addRoundRect(m_bRect,25);
    }

    m_holePath = QPainterPath();
    if ( m_hole ){
        double holeWidth = m_hole->width();
        QRectF holeBoundincRect(-holeWidth/2.0,-holeWidth/2.0,holeWidth,holeWidth);
        m_holePath.addEllipse(holeBoundincRect);
    }
}

void PadItem::setHole(HoleItem *hole){
    m_hole = hole;
    updateGeometry();
}

void PadItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget){
    Q_UNUSED(widget);
    if ( ! layer() )
        return;
    painter->setClipRect( option->exposedRect );

    QColor color;
    painter->setPen(Qt::NoPen);
    if ( isToolItem() )
        color = layer()->color().lighter();
    else
        color = layer()->color();

    if ( typonItemIsSelected() )
        color = Qt::white;

    painter->fillPath(m_shape,QBrush(color));

    if ( m_hole != 0 )
        painter->fillPath(m_holePath,qApp->workSpace()->grid->backgroundColor());
}
