#include "myitembase.h"
#include <QDebug>
//#include "trackitem/trackcontainer.h"
#include "global.h"
#include "paditem/paditem.h"
#include "tools/mytoolbase.h"

MYItemBase::MYItemBase(QGraphicsItem * parent, bool isGroup)
    : QGraphicsItem(parent), m_isGroup(isGroup){
    m_isGroundPlane = false; // is the item a groundplane?
    m_isToolItem = false; // tool items are items under mouse for creation tools
    //m_gndPlaneEffect = new GroundPlaneEffect;
    m_isSelected = false; // handle custom selection
    m_group = 0; // is the item a group one
    m_tool = 0; // pointer to the tool widget in order to update values when item is selected
    m_isUngroupable = false; // group is a pad or a component
    m_isOffScreen = false; // an item can be offscreen if it's layer has been deleted
    m_groupHandlePoint = QPointF(0,0); // if item is group, default handle point is boundingRect center, this is used to modify handle point
    m_isAngle = false;
    m_groundPlaneOffset = 50;
    setFlag(QGraphicsItem::ItemNegativeZStacksBehindParent);
    m_originalZValue = 0;
    setZValue(0);
}

MYItemBase::~MYItemBase(){
    //delete m_gndPlaneEffect;
    // if item has childs, set childs group as null
    for(int i = 0; i < m_childs.count(); i++)
        m_childs.at(i)->setGroup(0);
}

// items passed to stream are only toplevelitems!!
void MYItemBase::save(QDataStream &stream){
    if ( this->isGroup() ){
        stream << 0 << m_name << groupPos() << transform() << rotation() << isUngroupable() << m_childs.count();
        for ( int i = 0 ; i < m_childs.count(); i++ ){
            m_childs.at(i)->save(stream);
        }
    }else{
        this->save(stream);
    }
}

int MYItemBase::restore(QDataStream &stream){
    QString name;
    QPointF pos;
    QTransform transform;
    double rotation;
    bool unGroupable;

    stream >> name >> pos >> transform >> rotation >> unGroupable;

    setItemName(name);
    setPos(pos);
    setRotation(rotation);
    setTransform(transform,true);
    setUngroupable(unGroupable);

    int childCount;
    stream >> childCount;
    for (int i = 0; i < childCount; i++){
        int itemType;
        stream >> itemType;
        qApp->currentTypon()->restoreItem(stream,itemType, this);
    }

    return -1;
}

double MYItemBase::originalZValue(){
    return m_originalZValue;
}

bool MYItemBase::isAngle(){
   return m_isAngle;
}

bool MYItemBase::isOffScreen(){
    return m_isOffScreen;
}

void MYItemBase::setOffScreen(bool offScreen){
    m_isOffScreen = offScreen;
}

bool MYItemBase::isGroup(){
    return m_isGroup;
}

QList<MYItemBase *> MYItemBase::childs(){
    return m_childs;
}

// childs are added or removed from group but never deleted
// delete are always done by destroying the add undocommand
void MYItemBase::addToGroup(MYItemBase *item){
    m_childs.append(item);
    item->setGroup(this);
}

void MYItemBase::onAddToGroup(){

}

void MYItemBase::removeFromGroup(MYItemBase *item){
    m_childs.removeOne(item);
    item->setGroup(0);
}

void MYItemBase::setUngroupable(bool ungroupable){
    m_isUngroupable = ungroupable;
}

bool MYItemBase::isUngroupable(){
    return m_isUngroupable;
}

void MYItemBase::setGroup(MYItemBase *group){
    m_group = group;
}

MYItemBase *MYItemBase::group(){
    return m_group;
}

// return the top most group for an item
// return a pointer to the item itself if it has no group
MYItemBase *MYItemBase::topMostGroup(){
    MYItemBase *gr = this;
    while( gr->group() )
        gr = gr->group();
    return gr;
}

// update custom selection recursively
void MYItemBase::setTyponItemSelected(bool selected){
    m_isSelected = selected;
    for(int i = 0; i < m_childs.count(); i++){
        MYItemBase *child = m_childs.at(i);
        if ( child->isVisible() && child->isEnabled() && !m_isOffScreen )
            m_childs.at(i)->setTyponItemSelected(selected);
    }
    onTyponSelectedChange();
    if ( layer() ){
        if( typonItemIsSelected() )
            setZValue(zValue()+1);
        else
            setZValue(originalZValue());
    }
    update();
}

void MYItemBase::onTyponSelectedChange(){

}

bool MYItemBase::typonItemIsSelected(){
    return m_isSelected;
}

bool MYItemBase::isToolItem(){
    return m_isToolItem;
}

// set this item as a tool one, update all children recursivly
void MYItemBase::setToolItem(bool toolItem){
    m_isToolItem = toolItem;
    for(int i = 0; i < m_childs.count(); i++)
         m_childs.at(i)->setToolItem(toolItem);
}

void MYItemBase::setItemName(QString name){
    m_name = name;
}

QString MYItemBase::itemName(){
   return m_name;
}

void MYItemBase::setGroundPlaneOffset(double offset){
    m_groundPlaneOffset = offset;
    updateGeometry();
}

double MYItemBase::groundPlaneOffset(){
    return m_groundPlaneOffset;
}

// returns layer for this item, this is the top level item for an item
// no layer is set for a group as it is never added to the scene
// a custom layer function is provided  if needed in derived items
Layer *MYItemBase::layer() const{
    return dynamic_cast<Layer *>(topLevelItem());
}

// set layer, custom function can be used in derived item class as group does not have a kayer
void MYItemBase::setLayer(Layer *layer){
    if ( isGroundPlane() ){
        Layer *oldLayer = this->layer();
        oldLayer->groundPlanedItems()->removeOne(this);
    }
    setParentItem(layer);
    update();
}

bool MYItemBase::isGroundPlane() const{
    return m_isGroundPlane;
}

void MYItemBase::setGroundPlane(bool groundPlane){
    m_isGroundPlane = groundPlane;

    if ( groundPlane ){
        //setGraphicsEffect(m_gndPlaneEffect);
        m_originalZValue = -2;
        setZValue(-2);
    }else{
        //setGraphicsEffect(0);
        m_originalZValue = 0;
        setZValue(originalZValue());
    }

    if ( layer() ){
        if( typonItemIsSelected() )
            setZValue(zValue()+1);
        else
            setZValue(originalZValue());
    }
}

// return a list of path for each item colliding  with this item for given rect
// used to update ground plane drawing
QVector<QPainterPath> MYItemBase::collidingPaths(){
    QVector<QPainterPath> paths;
    if ( m_isGroundPlane ){
        foreach(QGraphicsItem *it, collidingItems()){
            MYItemBase *itBase = dynamic_cast<MYItemBase *>(it);
            Layer *itLay = dynamic_cast<Layer *>(it);
            if (itBase  && ( itBase != this )){
                if ( ! itBase->isGroundPlane() && !itLay && !itBase->isAngle() && !itBase->isToolItem() ){
                    if ( itBase->layer() == layer()
                       || itBase->layer() == qApp->currentTypon()->typonScene()->layers()->viaLayer() ){
                        QPainterPathStroker ps2;
                        ps2.setCapStyle(Qt::RoundCap);
                        ps2.setWidth(m_groundPlaneOffset);
                        ps2.setJoinStyle(Qt::RoundJoin);
                        ps2.setMiterLimit(0);
                        QPainterPath itPath = mapFromItem(itBase,itBase->shape());
                        paths.append(ps2.createStroke(itPath));
                    }
                }
            }
        }
    }
    return paths;
}

// rotate item and its children recursively if needed
void MYItemBase::rotate(double angle, QPointF center){
    if ( isGroup() ){
        foreach(MYItemBase* item, m_childs)
           item->rotate(angle,center);
    }else{
        if ( m_isAngle )
            return;

        double determinant = transform().determinant();
        if ( determinant < 0 )
            angle = -angle;

        QTransform t;
        t.translate(center.x(), center.y());
        t.rotate(angle);
        t.translate(-center.x(), -center.y());

        if ( determinant > 0)
            setPos(t.map(pos()));
        else
            setPos(t.inverted().map(pos()));
        setRotation(rotation() + angle);
    }
}

// mirror item and its children verticaly and recursively if needed
void MYItemBase::verticalMirror(QPointF center){
    if ( isGroup() ){
        foreach(MYItemBase* item, m_childs)
           item->verticalMirror(center);
    }else{
        if ( m_isAngle )
            return;

        double delta = center.y() - pos().y();

        QTransform t;
        t.translate(center.x(), -center.y());
        t.scale(1,-1);
        t.translate(-center.x(), -center.y());
        setTransform(t*transform());

        setPos(pos().x(),center.y()+delta);
    }
}

// mirror item and its children horizontally and recursively if needed
void MYItemBase::horizontalMirror(QPointF center){
    if ( isGroup() ){
        foreach(MYItemBase* item, m_childs)
           item->horizontalMirror(center);
    }else{
        if ( m_isAngle )
            return;

        double delta = center.x() - pos().x();

        QTransform t;
        t.translate(-center.x(), center.y());
        t.scale(-1,1);
        t.translate(-center.x(), -center.y());
        setTransform(t*transform());

        setPos(center.x()+delta,pos().y());
    }
}

// return the item scene bounding rect
// or the scene bounding rect of all children if item is a group ( a group has no boundingrect by itself)
QRectF MYItemBase::groupBoundingRect(){
    QRectF r = QRectF();
    if ( isGroup() ){
        foreach(MYItemBase* item, m_childs)
            r = r.united(item->groupBoundingRect());
    }else{
        r = sceneBoundingRect();
    }
    return r;
}

void MYItemBase::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget){
    Q_UNUSED(widget);
    Q_UNUSED(option)
    Q_UNUSED(painter)
}

void MYItemBase::updateGeometry(){

}

QRectF MYItemBase::boundingRect() const{
    QRectF bRect;
    /*for(int i = 0; i < m_childs.count(); i++)
        bRect = bRect.united(m_childs.at(i)->boundingRect());*/
    return bRect;
}

// if item is a group, provides a pointer to the widget toll used to edit it
// if item is of another type, it has its own function that handle a pointer to the correct widget type to edit it
void MYItemBase::setGroupTool(GroupTool *tool){
    m_tool = tool;
}

GroupTool *MYItemBase::groupTool(){
    return m_tool;
}

// recursive function wich finds if all children of an item is "available":
// an item is available if it is visible and enable and is not offscreen ( its layer has been deleted )
// used when a layer is removed, deacitvated or est unvisible => if all childs of a group are unavailable, group is removed from selection
bool MYItemBase::allChildsAreUnavailable(){
    bool result = true;
    if ( isGroup() ){
        for(int i = 0; i < m_childs.count(); i++){
            result = m_childs.at(i)->allChildsAreUnavailable();
            if ( !result )
                break;
        }
    }else{
        if ( isVisible() && isEnabled() && !m_isOffScreen )
            result = false;
    }
    return result;
}

// if item visibility or enability has change, update this value for all children of its top most gruop
QVariant MYItemBase::itemChange(GraphicsItemChange change, const QVariant &value){
    if (change == ItemEnabledHasChanged && scene()) {
        if ( isEnabled() )
            setTyponItemSelected(topMostGroup()->typonItemIsSelected());
        else
            setTyponItemSelected(false);

        if ( topMostGroup()->allChildsAreUnavailable() )
            qApp->currentTypon()->typonScene()->removeSelectedItem(topMostGroup());
    }
    if (change == ItemVisibleHasChanged && scene()) {
        if ( isVisible() )
            setTyponItemSelected(topMostGroup()->typonItemIsSelected());
        else
            setTyponItemSelected(false);

        if ( topMostGroup()->allChildsAreUnavailable() )
            qApp->currentTypon()->typonScene()->removeSelectedItem(topMostGroup());
    }    

    return QGraphicsItem::itemChange(change, value);
}

// move item by delta
// if item is a group, move all childs by delta recursively, used by select tool to move items
void MYItemBase::moveItemBy(QPointF delta){
    if ( isGroup() ){
        foreach(MYItemBase* item, m_childs)
            item->moveItemBy(delta);
    }else{
        if ( !m_isAngle )
            setPos(scenePos()+delta);
    }
}

// return the handle point of the item wich is bounding rect center by default
// or bounding rect center with a delta if handle point has been modified
QPointF MYItemBase::groupPos(){
    if ( isGroup() ){
       return groupBoundingRect().center() - m_groupHandlePoint;
    }else{
        return scenePos() - m_groupHandlePoint;
    }
}

// set item position, update group pos accordingly to its handle point if item is a group
void MYItemBase::setGroupPos(QPointF point){
    if ( isGroup() ){
        QPointF groupCenter = groupPos();
        foreach(MYItemBase* item, m_childs){
            QPointF delta = groupCenter - item->groupPos();
            item->setGroupPos(point-delta);
        }
    }else{
        setPos(point);
    }
}
