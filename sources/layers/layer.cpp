#include "layer.h"
#include <QVector>
#include <QDebug>
#include "global.h"
#include "items/myitembase.h"

Layer::Layer(QGraphicsItem *parent, QString name, QColor color)
    : QGraphicsItem(parent), m_layerName(name), m_color(color){

    m_isSystemLayer = false;
    m_isActive = false;
    setFlag(ItemHasNoContents);
    setFlag(ItemIsSelectable,false);
    setFlag(ItemIsFocusable,false);
    setFlag(ItemNegativeZStacksBehindParent);
    updateTransparency();
    setAcceptHoverEvents(true);
    m_groundPlanedItems = new QList<MYItemBase *>;
}

Layer::~Layer(){
    delete m_groundPlanedItems;
}

void Layer::save(QDataStream &stream){
    stream << m_layerName;
    stream << m_color;
}

void Layer::clearSelection(){
    foreach(QGraphicsItem *it, childItems()){
        MYItemBase *itBase = dynamic_cast<MYItemBase *>(it);
        if ( itBase )
            itBase->setTyponItemSelected(false);
    }
}

QList<MYItemBase *> *Layer::groundPlanedItems(){
    return m_groundPlanedItems;
}

void Layer::updateGroundPlanes(QRectF rect){
    Q_UNUSED(rect)
    for(int i = 0; i < m_groundPlanedItems->count(); i++){
        //if ( m_groundPlanedItems->at(i)->boundingRect().intersects(rect) )
            m_groundPlanedItems->at(i)->updateGeometry();
    }
}

int Layer::type() const {
    return Type;
}

void Layer::updateTransparency(){
    if ( qApp->workSpace()->semiTransparentTracks )
        setOpacity(0.7);
    else
        setOpacity(1);
}

Layer *Layer::clone(QGraphicsItem *parent){
    Layer *clone = new Layer(parent, m_layerName,m_color);
    clone->setSystem(m_isSystemLayer);
    return clone;
}

int Layer::layerZValue(){
    if ( this == qApp->currentTypon()->typonScene()->layers()->viaLayer() )
        return 65533;
    if ( this == qApp->currentTypon()->typonScene()->layers()->drillLayer() )
        return 65535;
    if ( this == qApp->currentTypon()->typonScene()->layers()->activeLayer() )
        return 65532;
    return qApp->currentTypon()->typonScene()->layers()->indexOf(this);
}

int Layer::realLayerZValue(){
    if ( this == qApp->currentTypon()->typonScene()->layers()->viaLayer() )
        return 0;
    if ( this == qApp->currentTypon()->typonScene()->layers()->drillLayer() )
        return 65535;
    return qApp->currentTypon()->typonScene()->layers()->indexOf(this);
}

void Layer::updateZValue(){
    setZValue(layerZValue());
}

void Layer::setSystem(bool systemLayer){
    m_isSystemLayer = systemLayer;
}

bool Layer::isSystem(){
    return m_isSystemLayer;
}

void Layer::setColor(QColor color){
    m_color = color;
}

QColor Layer::color(){
    return m_color;
}

void Layer::setName(QString name){
    m_layerName = name;
}

QString Layer::name(){
     return m_layerName;
}

QRectF Layer::boundingRect() const{
    QRectF bRect;
    foreach(QGraphicsItem *it, childItems()){
        MYItemBase *itBase = dynamic_cast<MYItemBase *>(it);
        if ( itBase )
            bRect = bRect.united(itBase->sceneBoundingRect());
    }
    return bRect;
}

void  Layer::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget){
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)
}
