#include "padgroup.h"
#include "global.h"
#include <QDebug>

PadGroup::PadGroup(MYItemBase *parent, bool incrementCounter)
    : MYItemBase(parent,true){
    setFlag(ItemHasNoContents);

    m_layer = 0;
    m_isVia = false;
    m_padTool = 0;

    m_hole = new HoleItem(0);
    m_pad = new PadItem(0);
    addToGroup(m_hole);
    addToGroup(m_pad);
    setHasHole(true);
    setUngroupable(true);


    // if the item is a "real" one, not tool item or temporary item for undo/redo commands, increment counter
    if ( qApp->currentTypon() && incrementCounter )
        setItemName(QObject::tr("Pad %1").arg(QString::number(++qApp->currentTypon()->padCounter)));
}

PadGroup::~PadGroup(){
}

void PadGroup::save(QDataStream &stream){
    int layerZValue;
    if ( isVia() )
        layerZValue = qApp->currentTypon()->typonScene()->layers()->viaLayer()->realLayerZValue();
    else
        layerZValue = m_layer->realLayerZValue();
    stream << 1 << m_name << pos() << m_pad->pos() << m_hole->pos()
           << transform() << m_pad->transform() << m_hole->transform()
           << rotation()  << m_pad->rotation() << m_hole->rotation()
           << layerZValue
           << (int)m_pad->padType() << m_pad->width() << m_hasHole << m_hole->width() << m_isVia;
}

int PadGroup::restore(QDataStream &stream){
    QString name;
    QPointF pos;
    QPointF padPos;
    QPointF holePos;
    QTransform transform;
    QTransform padTransform;
    QTransform holeTransform;
    double rotation;
    double padRotation;
    double holeRotation;
    int layerZValue;
    int padType;
    double width;
    bool hasHole;
    double holeWidth;
    bool isVia;        

    stream >> name >> pos >> padPos >> holePos >> transform
            >> padTransform >> holeTransform
             >> rotation >>  padRotation >>  holeRotation
            >> layerZValue >> padType >> width
                           >> hasHole >> holeWidth >> isVia;

    setItemName(name);
    setGroupPos(pos);
    m_pad->setPos(padPos);
    m_hole->setPos(holePos);
    setRotation(rotation);
    m_pad->setRotation(holeRotation);
    m_hole->setRotation(holeRotation);
    setTransform(transform,true);
    m_pad->setTransform(padTransform,true);
    m_hole->setTransform(holeTransform,true);

    setType((PadItem::PadType)padType);
    m_pad->setWidth(width);
    setHasHole(hasHole);
    setHoleWidth(holeWidth);
    setVia(isVia);

    return layerZValue;
}

// manage a pointer to the widget that can edit  this item
void PadGroup::setTool(PadTool *tool){
    m_padTool = tool;
}

PadTool *PadGroup::tool(){
    return m_padTool;
}

PadGroup *PadGroup::clone(){
    PadGroup *grp = new PadGroup(0,true);
    grp->pad()->setType(m_pad->padType());
    grp->pad()->setWidth(m_pad->width());
    grp->pad()->setItemName(m_pad->itemName());
    grp->hole()->setWidth(m_hole->width());
    grp->hole()->setItemName(m_hole->itemName());
    grp->setHasHole(m_hasHole);
    grp->setType(m_pad->padType());
    grp->pad()->setTransform(m_pad->transform());
    grp->pad()->setRotation(m_pad->rotation());
    grp->hole()->setTransform(m_hole->transform());
    grp->hole()->setRotation(m_hole->rotation());
    grp->setLayer(m_layer);
    grp->setVia(m_isVia);
    return grp;
}

// update the pad "via state"
void PadGroup::setVia(bool via){
    m_isVia = via;

    // check if there is a valid typon
    if ( qApp->currentTypon() ){
        if ( isVia() ){
            setLayer(qApp->currentTypon()->typonScene()->layers()->viaLayer());
        }else{
            // if m_layer is an invalid pointer, original layer for this pad has been deleted while it was in via state
            // update layer to new active one
            if ( !qApp->currentTypon()->typonScene()->layers()->contains(m_layer) )
                setLayer(qApp->currentTypon()->typonScene()->layers()->activeLayer());
            else
               setLayer(m_layer);
        }
    }
}

bool PadGroup::isVia(){
    return m_isVia;
}

void PadGroup::setHoleWidth(double width){
    m_hole->setWidth(width);
    m_pad->updateGeometry();
}

PadItem *PadGroup::pad(){
    return m_pad;
}

HoleItem *PadGroup::hole(){
    return m_hole;
}

void PadGroup::setHasHole(bool hasHole){
    m_hasHole = hasHole;
    if ( hasHole )
        m_pad->setHole(m_hole);
    else
        m_pad->setHole(0);
    setType(m_pad->padType());
}

bool PadGroup::hasHole(){
    return m_hasHole;
}

// set pad layer
void PadGroup::setLayer(Layer *layer){
    if ( ! qApp->currentTypon() )
        return;
    // update internal layer pointer if  pad is not a via
    if ( layer != qApp->currentTypon()->typonScene()->layers()->viaLayer() )
        m_layer = layer;
    // if layer is   we want to remove pad from scene
    if ( layer == 0 ){
        if ( m_pad->parentItem() )
            qApp->currentTypon()->typonScene()->removeTyponItem(m_pad);
        if ( m_hole->parentItem() )
            qApp->currentTypon()->typonScene()->removeTyponItem(m_hole);
    }else{
        // if pad is not only a hole
        if ( m_pad->padType() != PadItem::Hole ){
            // add pad to layer and update its z value
            m_pad->setParentItem(layer);
        }
        // if pad has a hole or is only a hole
        if ( m_hasHole || m_pad->padType() == PadItem::Hole){
            // add the hole to the drill layer and update z value
            m_hole->setParentItem(qApp->currentTypon()->typonScene()->layers()->drillLayer());
        }
    }
}

// update pad type
void PadGroup::setType(PadItem::PadType type){
    m_pad->setType(type);
    if ( m_layer == 0 )
        return;
    // if pas is only a hole
    if ( type == PadItem::Hole ){
        // remove copper pad part
        if ( m_pad->parentItem() )
            qApp->currentTypon()->typonScene()->removeTyponItem(m_pad);
        // if hole was deactivated, activate it
        if ( ! m_hole->parentItem() )
            m_hole->setParentItem(qApp->currentTypon()->typonScene()->layers()->drillLayer());
    }else{
        // pad is not only a hole
        // if pas is not on the scene, add it
        if ( ! m_pad->parentItem() )
            m_pad->setParentItem(m_layer);
        // if pad has no hole but hole is activated, remove it
        if ( !m_hasHole && m_hole->parentItem() )
            qApp->currentTypon()->typonScene()->removeTyponItem(m_hole);
        // if pad has hole but hole is not on the scene, add it
        if ( m_hasHole && !m_hole->parentItem() )
            m_hole->setParentItem(qApp->currentTypon()->typonScene()->layers()->drillLayer());
    }
}
