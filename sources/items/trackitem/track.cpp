#include "track.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "global.h"
#include <QDebug>
#include "angle.h"

Track::Track(MYItemBase *parent, bool incrementCounter)
    : MYItemBase(parent){
    m_width = 10;
    m_type = Flat;
    m_originalZValue = 2;
    setZValue(2);
    // if the item is a "real" one, not tool item or temporary item for undo/redo commands, increment counter
    if ( qApp->currentTypon() && incrementCounter )
        setItemName(QObject::tr("Track %1").arg(QString::number(++qApp->currentTypon()->trackCounter)));
}

void Track::save(QDataStream &stream){
    stream << 3 << m_name << pos() << transform() << rotation() << layer()->realLayerZValue()
    << (int)m_type << m_width
    << m_angles.count();

    foreach (Angle *angle, m_angles)
        stream << angle->scenePos();
}

int Track::restore(QDataStream &stream){
    QString name;
    QPointF pos;
    QTransform transform;
    double rotation;
    int layerZValue;
    int trackType;
    double width;

    stream >> name >> pos >> transform >> rotation >> layerZValue >> trackType >> width;
    setItemName(name);
    setPos(pos);
    setRotation(rotation);
    setTransform(transform,true);

    setType((TrackType)trackType);
    setWidth(width);

    int anglesCount;
    stream >> anglesCount;

    for(int i = 0; i < anglesCount; i++){
        QPointF anglePos;
        stream >> anglePos;
        appendAngle(anglePos,false);
    }

    return layerZValue;
}

// get the nearest point on the "base" line from point
QPointF Track::linePoint(QPointF point, int index){
    // get segment of track where point is
    // if index is -1, whe need to compute the index from point otherwise the segment is given by the index parameter
    int i = index;
    if ( index == - 1 )
        i = indexOfPoint(point);

    if ( i == -1 )
        return QPointF(0,0);

    QPointF pt = mapFromParent(point);
    QLineF trackLine;
    if ( i == -2 ){
        trackLine.setP1(m_angles.last()->pos());
        trackLine.setP2(m_angles.first()->pos());
    }else{
        trackLine.setP1(m_angles.at(i-1)->pos());
        trackLine.setP2(m_angles.at(i)->pos());
    }

    double APx = pt.x() - trackLine.x1();
    double APy = pt.y() - trackLine.y1();
    double ABx = trackLine.x2() - trackLine.x1();
    double ABy = trackLine.y2() - trackLine.y1();
    double magAB2 = ABx*ABx + ABy*ABy;
    double ABdotAP = ABx*APx + ABy*APy;
    double t = ABdotAP / magAB2;

    QPointF newPoint;

    if ( t < 0) {
        newPoint = trackLine.p1();
    }else if (t > 1){
        newPoint = trackLine.p2();
    }else{
        newPoint.setX(trackLine.x1() + ABx*t);
        newPoint.setY(trackLine.y1() + ABy*t);
    }

    return newPoint;
}

// get the segment of track in which point is contained
// could search on last segment if item is polygon item
// Return values:
// -1 => Not found
// -2 => point on last segment
// else => Segment containing point
int Track::indexOfPoint(QPointF point){
    int index = -1;

    QPainterPathStroker ps;
    ps.setCapStyle(Qt::RoundCap);
    ps.setWidth(m_width);
    ps.setJoinStyle(Qt::RoundJoin);
    ps.setMiterLimit(0);

    // get point position
    point = mapFromParent(point);

    // iterate all segments of track
    for(int i =0; i < m_angles.count() -1; i++){
        QPainterPath path;
        // generate linePath from the current segment index
        generatePath(path,m_angles.at(i),m_angles.at(i+1));
        // create the stroke with width parameter
        path = ps.createStroke(path);
        // test if point is contained in this segment
        if ( path.contains(point) ){
            index = i+1;
            break;
        }
    }

    //search on last segment
    //could be in a polygon item
    QPainterPath path;
    // generate linePath from the current segment index
    generatePath(path,m_angles.last(),m_angles.first());
    // create the stroke with width parameter
    path = ps.createStroke(path);
    // test if point is contained in this segment
    if ( path.contains(point) ){
        index = -2;
    }

    return index;
}

// generate path from the segment contain beetween angle1 and angle2
void Track::generatePath(QPainterPath &path, Angle *angle1, Angle *angle2){
    if (m_type == Flat){
        path.moveTo(angle1->pos());
        path.lineTo(angle2->pos());
    }

    if (m_type == Curved){
        path.moveTo(angle1->pos());
        QRectF rect(angle2->pos(),angle2->pos());
        rect = rect.normalized();
        QPointF pt1(rect.center().x(),angle1->pos().y());
        QPointF pt2(angle2->pos().x(),rect.center().y());
        path.cubicTo(pt1,pt2,angle2->pos());
    }

    if (m_type == Curved2){
        path.moveTo(angle1->pos());
        QRectF rect(angle2->pos(),angle2->pos());
        rect = rect.normalized();
        QPointF pt1(angle1->pos().x(),rect.center().y());
        QPointF pt2(rect.center().x(),angle2->pos().y());
        path.cubicTo(pt1,pt2,angle2->pos());
    }
}

Track *Track::clone(){
    Track *clone = new Track(0,true);
    clone->prepareGeometryChange();

    foreach (Angle *angle, m_angles)
        clone->appendAngle(angle->scenePos(),false);

    clone->setType(m_type);
    clone->setWidth(m_width);
    return clone;
}

void Track::setTool(TrackTool *tool){
    m_tool = tool;
}

TrackTool *Track::tool(){
    return m_tool;
}

// when selection changed (could be by undo/redoing),  update angles visibility and selection
// if track is part of a group, do not show angles
void Track::onTyponSelectedChange(){
    if ( group() ){
        hideAngles();
        return;
    }
    foreach (Angle *angle, m_angles){
        angle->setVisible(typonItemIsSelected());
        angle->setTyponItemSelected(typonItemIsSelected());
    }
}

void Track::hideAngles(){
    foreach (Angle *angle, m_angles)
        angle->setVisible(false);
}

void Track::onAddToGroup(){
    hideAngles();
}

// check if the given angle is on an extremity of the track
bool Track::angleIsOnExtremity(Angle *angle){
    bool onExtremity = false;
    if ( isFirstAngle(angle) || isLastAngle(angle) )
        onExtremity = true;
    return onExtremity;
}

bool Track::isFirstAngle(Angle *angle){
    return angle == m_angles.first();
}

bool Track::isLastAngle(Angle *angle){
    return angle == m_angles.last();
}

QVector<Angle *> Track::angles(){
    return m_angles;
}

// append the given track to this track
void Track::append(Track *track, bool reversed){
    if ( reversed ){ // append given track reversly
        for(int i = track->angles().count() - 1; i >= 0; i--)
            appendAngle(track->angles().at(i)->groupPos());
    }else{ // append given track in normal order
        for(int i = 0; i < track->angles().count(); i++)
            appendAngle(track->angles().at(i)->groupPos());
    }
    onTyponSelectedChange();
}

// append a new angle at given pos
// if checklastpos is true, angle will be added only if pos is fifferent from this last angle pos
void Track::appendAngle(QPointF pos, bool checkLastPos){
    if ( m_angles.count() && checkLastPos ){
        if ( qFuzzyCompare(QVector2D(m_angles.last()->scenePos()),QVector2D(pos)))
            return;
    }
    Angle *angle = new Angle(this);
    angle->setItemName("angle " + itemName());
    angle->setParentItem(this);
    angle->setGroupPos(mapFromParent(pos));
    angle->setWidth(m_width);
    m_angles.append(angle);
    angle->setVisible(false);

    updateGeometry();
}

// append an existing angle
// if checklastpos is true, angle will be added only if pos is fifferent from this last angle pos
void Track::appendAngle(Angle *angle, bool checkLastPos){
    if ( m_angles.count() && checkLastPos ){
        if ( qFuzzyCompare(QVector2D(m_angles.last()->scenePos()),QVector2D(angle->scenePos())))
            return;
    }
    Track *angleTrack = dynamic_cast<Track *>(angle->parentItem());
    if ( angleTrack )
        angle->setPos(mapFromItem(angleTrack,angle->pos()));
    angle->setParentItem(this);
    angle->setWidth(m_width);
    m_angles.append(angle);
    angle->setVisible(false);

    updateGeometry();
}

// prepend an existing angle
// if checklastpos is true, angle will be added only if pos is fifferent from this last angle pos
void Track::prependAngle(Angle *angle, bool checkLastPos){
    if ( m_angles.count() && checkLastPos ){
        if ( qFuzzyCompare(QVector2D(m_angles.first()->scenePos()),QVector2D(angle->scenePos())))
            return;
    }

    Track *angleTrack = dynamic_cast<Track *>(angle->parentItem());
    if ( angleTrack )
        angle->setPos(mapFromItem(angleTrack,angle->pos()));
    angle->setParentItem(this);
    angle->setWidth(m_width);
    m_angles.prepend(angle);
    angle->setVisible(false);

    updateGeometry();
}

// insert an existing angle (the angle is created by the insertAngleUndoCommand)
void Track::insertAngle(Angle *angle, int index, QPointF pos){
    angle->setParentItem(this);
    angle->setGroupPos(pos);
    angle->setWidth(m_width);
    if ( index >= 0 )
        m_angles.insert(index,angle);
    else
        m_angles.append(angle);
    angle->setTyponItemSelected(typonItemIsSelected());
    angle->setVisible(typonItemIsSelected());

    updateGeometry();
}

// remove one angle from this angles, delete it if necessary
void Track::removeAngle(Angle *angle, bool deleteAfterRemove){
    m_angles.removeOne(angle);
    if ( scene() && angle->parentItem() == this )
        qApp->currentTypon()->typonScene()->removeTyponItem(angle);
    if ( deleteAfterRemove )
        delete angle;
    updateGeometry();
}

void Track::clearTrack(){
    foreach(Angle *angle, m_angles)
        delete angle;
    m_angles.clear();
    updateGeometry();
}

void Track::updateAnglePos(Angle *angle, QPointF pos){
    angle->setGroupPos(mapFromParent(pos));

    updateGeometry();
}

void Track::setType(TrackType type){
    m_type = type;
    updateGeometry();
}

Track::TrackType Track::trackType(){
    return m_type;
}

QPainterPath Track::shape() const{
    return m_shape;
}

void Track::setWidth(double width){
    m_width = width;
    for (int i = 0; i < m_angles.count() ; i++ )
        m_angles.at(i)->setWidth(width);
    updateGeometry();
}

double Track::width(){
    return m_width;
}

QRectF Track::boundingRect() const{
    if ( ! isVisible() )
        return QRectF();
    else
        return m_bRect;
}

void Track::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget){
    Q_UNUSED(widget);
    Q_UNUSED(option)
    if ( ! layer() )
        return;
    painter->setClipRect( option->exposedRect );
    painter->setPen(Qt::NoPen);

    QColor color;
    if ( isToolItem() )
        color = layer()->color().lighter();
    else
        color = layer()->color();

    if ( m_isSelected )
        color = Qt::white;

    painter->fillPath(m_shape,color);
}

void Track::updateGeometry(){
    prepareGeometryChange();

    m_shape = QPainterPath();
    m_bRect = QRectF();

    if ( m_angles.count() < 2 )
        return;

    double halhWidth = m_width /2.0;
    m_bRect = QRectF(m_angles.at(0)->pos(),m_angles.at(1)->pos()).normalized().adjusted(-halhWidth,-halhWidth,halhWidth,halhWidth);

    QPainterPathStroker ps;
    ps.setCapStyle(Qt::RoundCap);
    ps.setWidth(m_width);
    ps.setJoinStyle(Qt::RoundJoin);
    ps.setMiterLimit(0);


    QPainterPath path;

    // generate path from each track segment
    for ( int i = 0; i < m_angles.count() -1; i++ )
        generatePath(path,m_angles.at(i),m_angles.at(i+1));

    m_path = path;

    m_shape = ps.createStroke(path);
    m_bRect = m_shape.boundingRect();
}
