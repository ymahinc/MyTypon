#include "mytyponview.h"
#include "mytyponscene.h"
#include "global.h"
#include <QMouseEvent>
#include <QDebug>
#include <QScrollBar>
#include "mytyponscrollbar.h"
#include <QStyleOptionGraphicsItem>

MYTyponView::MYTyponView(MYTyponScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent) {
    m_scene = scene;
    init();
}

MYTyponView::~MYTyponView(){

}

// init drawing zone settings
void MYTyponView::init() {
    m_cursorHasQuitViewport = true;
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
    m_dynamicGridFactor = 1.0;
    m_cursorIsSet = false;
    setFocusPolicy(Qt::StrongFocus);
    m_momentaryDisableSnap = false;
    setViewportUpdateMode(MinimalViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing,true);
    setCacheMode(QGraphicsView::CacheBackground);
    m_lastMousePos = QPoint(0,0);
    m_lastMouseScenePos = QPointF(0,0);
    m_cursorIsInViewPort = false;
    setMouseTracking(true);
    MYTyponScrollBar *hScrollBar = new MYTyponScrollBar(this);
    setHorizontalScrollBar(hScrollBar);
    MYTyponScrollBar *vScrollBar = new MYTyponScrollBar(this);
    setVerticalScrollBar(vScrollBar);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

//return last mouse pos in viewport coordinates
QPoint MYTyponView::lastMousePos(){
    return m_lastMousePos;
}

 MYTyponScene *MYTyponView::typonScene(){
     return m_scene;
 }

// called from main ui when the view is scaled to update lines under cursor positions
// if called when autoscroll is happening, update position even if mouse is outside viewport
void MYTyponView::updateLastMousePos(bool extending){
    if ( ! qApp->workSpace()->linesUnderCursor && !extending )
        return;
    QPoint newPoint = mapFromGlobal(QCursor::pos());
    // don't draw lines if mouse is outside viewport
    // could happen if cursor is outside viweport at application load
    if ( ! viewport()->rect().contains(newPoint) && !extending ){
        leaveViewport();
        return;
    }
    // call the function wich updates lines whith new cursor pos
    updateMousePos(newPoint, true);
}

// hide lines when cursor leaves drawing zone
void MYTyponView::leaveViewport(QEvent *event){
    m_momentaryDisableSnap = false;
    bool oldcursorIsInViewPort = m_cursorIsInViewPort;
    m_cursorIsInViewPort = false;
    // if we have leave viewport and avoid having multiple elave events
    // update scene and tell the current tool about it
    if ( oldcursorIsInViewPort != m_cursorIsInViewPort ){
        m_scene->invalidate(mapToScene(viewport()->rect()).boundingRect(),QGraphicsScene::AllLayers);
        if ( ! m_cursorHasQuitViewport )
            qApp->mainTool()->handleLeaveEvent(event);
    }
    m_cursorHasQuitViewport = true;
    emit viewportLeaved(); 
}

bool MYTyponView::cursorIsInViewport(){
    return m_cursorIsInViewPort;
}

// return a value representing the scale factor
double MYTyponView::levelOfdetails(){
    return QStyleOptionGraphicsItem::levelOfDetailFromTransform(QGraphicsView::transform());
}

QPointF MYTyponView::nearestGridPoint(QPointF point){
    // find if we need to snap to grid
    bool disableSnap = !qApp->workSpace()->snapToGrid;
    if ( ! disableSnap && m_momentaryDisableSnap )
        disableSnap = true;

    return qApp->workSpace()->grid->nearestPoint(point,
               qApp->workSpace()->originPos(qApp->currentTypon()->typonSize()),
               disableSnap, m_dynamicGridFactor);
}

// update lines under cursor chen a mouse event occured
// m_lastMousePos is in viewport coordinates
// m_lastMouseScenePos is in scene coordinates
void MYTyponView::updateMousePos(QPoint pos, bool fromWheel){
    // update cursor if not already set
    // could happen at startup as tool is created before first typon
    if ( ! m_cursorIsSet ){
        m_cursorIsSet = true;
        setCursor(qApp->mainTool()->cursor());
    }
    if ( sceneRect().contains(mapToScene(pos)) ){ // only if we are in the drawing zone
        m_cursorIsInViewPort = true;
        // do not update if pos has not changed, unless if the change occured after a mouse whell event
        // in this case previous pos in viewport coordinate is the same as new pos but scene pos need to be recomputed
        if ( pos == m_lastMousePos && !fromWheel )
            return;

        // get new mouse position
        QPointF newPos = nearestGridPoint(mapToScene(pos));
        if ( qApp->workSpace()->linesUnderCursor ){ // if lines, redraw old and new position
            QRectF viewPortSceneRect = mapToScene(viewport()->rect()).boundingRect();
            m_scene->invalidate(QRectF(m_lastMouseScenePos.x(),viewPortSceneRect.top(),1,viewPortSceneRect.height()).adjusted(-0.5,-0.5,0.5,0.5),QGraphicsScene::AllLayers);
            m_scene->invalidate(QRectF(newPos.x(),viewPortSceneRect.top(),1,viewPortSceneRect.height()).adjusted(-0.5,-0.5,0.5,0.5),QGraphicsScene::AllLayers);
            m_scene->invalidate(QRectF(viewPortSceneRect.left(),m_lastMouseScenePos.y(),viewPortSceneRect.width(),1).adjusted(-0.5,-0.5,0.5,0.5),QGraphicsScene::AllLayers);
            m_scene->invalidate(QRectF(viewPortSceneRect.left(),newPos.y(),viewPortSceneRect.width(),1).adjusted(-0.5,-0.5,0.5,0.5),QGraphicsScene::AllLayers);
        }

        // update the last positions of the mouse to be able to update it at the next move
        m_lastMouseScenePos = newPos;
        m_lastMousePos =  pos;

        // emit signals to update status labels and rulers line position
        emit mousePosHasChangedScenePos(newPos);

        QMouseEvent mouseEvent(QEvent::MouseMove,m_lastMousePos,Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
        qApp->mainTool()->handleMouseMoveEvent(&mouseEvent);
    }else {
        // cursor is outside the drawing zone, hide the lines
        m_cursorIsInViewPort = false;
        m_scene->invalidate(mapToScene(viewport()->rect()).boundingRect(),QGraphicsScene::AllLayers);
        emit leaveViewport();
        // tell the tool <e have leave viewport, no mouse event to give
        qApp->mainTool()->handleLeaveEvent(0);
    }
}

QRectF MYTyponView::typonRect(){
    QRectF typonRect = QRectF(0,0,qApp->currentTypon()->typonSize().width(),
                              qApp->currentTypon()->typonSize().height());
    typonRect.translate(25000.0 - typonRect.width() / 2.0, 25000.0 - typonRect.height() / 2.0);
    return typonRect;
}

QRectF MYTyponView::sceneViewportRect(){
    return mapToScene(viewport()->rect()).boundingRect();
}

// draw the backround of the drawing zone (the grid)
void MYTyponView::drawBackground (QPainter* painter, const QRectF &rect){
    // get rect to be redrawn, not drawing outside the scene rect
    QRectF rf = rect.intersected(sceneRect()).adjusted(-1.0,-1.0,1.0,1.0);
    painter->fillRect(rf,qApp->workSpace()->grid->backgroundColor());
    painter->setClipRect(sceneRect());
    drawBackgroundContent(painter,rf);

}

void MYTyponView::drawBackgroundContent(QPainter* painter, const QRectF &rect){
    QPen pen;
    pen.setCosmetic(true);

    if ( qApp->workSpace()->grid->enabled() ) {
        double lvlOfDetail = levelOfdetails();
        // get grid steps and calculate the zoom factor
        m_dynamicGridFactor = 1.0;
        double xRawStep = qApp->workSpace()->grid->xStepGrid();
        double yRawStep =  qApp->workSpace()->grid->yStepGrid();

        if ( qApp->workSpace()->grid->dynamicGrid() ){
            double xDetails = lvlOfDetail * xRawStep;
            double yDetails = lvlOfDetail * yRawStep;
            while (yDetails > 25 || xDetails> 25) {
                xRawStep /= 2.0;
                yRawStep /= 2.0;
                m_dynamicGridFactor /= 2.0;
                xDetails = lvlOfDetail * xRawStep;
                yDetails = lvlOfDetail * yRawStep;
            }
            while (yDetails < 12.5 || xDetails < 12.5) {
                xRawStep *= 2.0;
                yRawStep *= 2.0;
                m_dynamicGridFactor *= 2.0;
                xDetails = lvlOfDetail * xRawStep;
                yDetails = lvlOfDetail * yRawStep;
            }
        }

        // draw grid only if details are big enough or if grid is dynamic
        if ( /*qApp->workSpace()->grid->dynamicGrid() ||*/ !((lvlOfDetail * xRawStep < 12.5) || (lvlOfDetail * yRawStep < 12.5))){

            // set grid color
            pen.setColor(qApp->workSpace()->grid->gridColor());
            painter->setPen(pen);

            // get origin point offsets
            double xOffset = fmod(qApp->workSpace()->originPos(qApp->currentTypon()->typonSize()).x(), xRawStep);
            double yOffset = fmod(qApp->workSpace()->originPos(qApp->currentTypon()->typonSize()).y(), yRawStep);

            // get offset of the viewport boundaries with grid steps
            double left = rect.left() - fmod(rect.left(), xRawStep) + xOffset;
            double top = rect.top() - fmod(rect.top(), yRawStep) + yOffset;

            // if grid type is dot, draw dots
            if ( qApp->workSpace()->grid->gridType() == MYTyponGrid::Dot ) {
                QVarLengthArray<QPointF> points;
                for (double x = left; x < rect.right(); x += xRawStep){
                    for (double y = top; y < rect.bottom(); y += yRawStep){
                        points.append(QPointF(x,y));
                    }
                }
                painter->drawPoints(points.data(), points.size());
            }

            // if grid type is lines, draw lines
            if ( qApp->workSpace()->grid->gridType() == MYTyponGrid::Line ) {
                QVarLengthArray<QLineF> lines;
                for (double x = left; x < rect.right(); x += xRawStep){
                    lines.append(QLineF(x,rect.top(),x,rect.bottom()));
                }
                for (double y = top; y < rect.bottom(); y += yRawStep){
                    lines.append(QLineF(rect.left(),y,rect.right(),y));
                }
                painter->drawLines(lines.data(), lines.size());
            }

            if ( qApp->workSpace()->grid->mainLines() ){
                pen.setColor(qApp->workSpace()->grid->gridColor().lighter(150));
                painter->setPen(pen);

                // get grid steps and calculate the zoom factor
                xRawStep *= qApp->workSpace()->grid->mainLinesSubDivs();
                yRawStep *= qApp->workSpace()->grid->mainLinesSubDivs();

                xOffset = fmod(qApp->workSpace()->originPos(qApp->currentTypon()->typonSize()).x(), xRawStep);
                yOffset = fmod(qApp->workSpace()->originPos(qApp->currentTypon()->typonSize()).y(), yRawStep);

                // get offset of the viewport boundaries with grid steps
                left = rect.left() - fmod(rect.left(), xRawStep) + xOffset;
                top = rect.top() - fmod(rect.top(), yRawStep) + yOffset;

                QVarLengthArray<QLineF> lines;
                for (double x = left; x < rect.right(); x += xRawStep){
                    lines.append(QLineF(x,rect.top(),x,rect.bottom()));
                }
                for (double y = top; y < rect.bottom(); y += yRawStep){
                    lines.append(QLineF(rect.left(),y,rect.right(),y));
                }
                painter->drawLines(lines.data(), lines.size());
            }
        }
    }

    painter->setPen(pen);

    // draw the board rect according to the typon size
    QRectF typonR = typonRect();

    if ( typonR.intersects(rect.toRect()) && ! typonR.contains(rect.toRect()) ){
        pen.setColor(Qt::yellow);
        painter->setPen(pen);
        painter->drawRect(typonR);
    }
}

//draw the lines under the mouse cursor
void MYTyponView::drawForeground(QPainter *painter, const QRectF &rect) {
    if ( !sceneRect().contains(rect) ){
        QPainterPath p;
        p.addRect(rect);
        QPainterPath sceneRectPath;
        sceneRectPath.addRect(sceneRect());
        painter->fillPath(p.subtracted(sceneRectPath),Qt::white);
    }

    // get rect to redraw
    QRectF rf = rect.intersected(sceneRect()).adjusted(-0.5,-0.5,0.5,0.5);
    painter->setClipRect(sceneRect());

    if ( ! cursorIsInViewport() )
        return;
    if ( ! qApp->workSpace()->linesUnderCursor )
        return;

    // set pen color
    QPen pen;
    pen.setCosmetic(true);
    pen.setColor(Qt::darkCyan);
    painter->setPen(pen);

    // find if we need to snap to grid
    bool disableSnap = !qApp->workSpace()->snapToGrid;
    if ( ! disableSnap && m_momentaryDisableSnap )
        disableSnap = true;

    if ( qApp->mainTool()->needToDrawRubberBand() )
        painter->drawRect(qApp->mainTool()->rubberBandRect());

    if ( qApp->mainTool()->needToBeDrawn() ){
        pen.setColor(Qt::blue);
        painter->setPen(pen);
        painter->drawPath(qApp->mainTool()->path());
    }

    pen.setColor(Qt::lightGray);
    painter->setPen(pen);
    // draw lines according to last mouse position
    QVarLengthArray<QLineF> lines;
    lines.append(QLineF(m_lastMouseScenePos.x(),rf.top(),m_lastMouseScenePos.x(),rf.bottom()));
    lines.append(QLineF(rf.left(),m_lastMouseScenePos.y(),rf.right(),m_lastMouseScenePos.y()));
    painter->drawLines(lines.data(), lines.size());
}

// disable snap momentarly (ie control key is pressed)
void MYTyponView::momentaryDisableSnap(bool disable){
    m_momentaryDisableSnap = disable;
}

void MYTyponView::mousePressEvent(QMouseEvent *mouseEvent){
    qApp->mainTool()->handleMousePressEvent(mouseEvent);
    if ( mouseEvent->isAccepted() )
        QGraphicsView::mousePressEvent(mouseEvent);
}

void MYTyponView::mouseReleaseEvent(QMouseEvent *mouseEvent){
    qApp->mainTool()->handleMouseReleaseEvent(mouseEvent);
    if ( mouseEvent->isAccepted() )
        QGraphicsView::mouseReleaseEvent(mouseEvent);
}

void MYTyponView::mouseDoubleClickEvent(QMouseEvent *event){
    MYTyponView::mousePressEvent(event);
}

// on mouse move, call default behaviour, then custom function to update display
void MYTyponView::mouseMoveEvent(QMouseEvent *mouseEvent){
    QGraphicsView::mouseMoveEvent(mouseEvent);
    if ( m_cursorHasQuitViewport ){
        m_cursorHasQuitViewport = false;
        qApp->mainTool()->handleEnterEvent(mouseEvent);
    }
    qApp->mainTool()->handleMouseMoveEvent(mouseEvent);
    updateMousePos(mouseEvent->pos());
}

void MYTyponView::keyPressEvent(QKeyEvent * event){
    qApp->mainTool()->handleKeyPressEvent(event);
}

void MYTyponView::keyReleaseEvent(QKeyEvent * event){
    qApp->mainTool()->handleKeyReleaseEvent(event);
}

void MYTyponView::enterEvent(QEvent *event){
    m_cursorIsInViewPort = true;
    QGraphicsView::enterEvent(event);
    setFocus();
}

void MYTyponView::leaveEvent(QEvent *event){
    leaveViewport(event);
    QGraphicsView::leaveEvent(event);
}

void MYTyponView::wheelEvent(QWheelEvent *event){
    // if the current tool dont allow wheel events, return
    if ( ! qApp->mainTool()->acceptWheelEvents() ){
        event->ignore();
        return;
    }

    // if it accepts wheel event and mouse wheel is not used to zoom, call default wheel behaviour
    if ( qApp->mainTool()->acceptWheelEvents() && !qApp->mainTool()->midZoomModOn() )
            QGraphicsView::wheelEvent(event);
    else
        event->ignore();

    updateMousePos(event->pos(),true);
    qApp->mainTool()->handleWheelEvent(event);
}

bool MYTyponView::canZoomIn(){
    return levelOfdetails() < 16;
}

bool MYTyponView::canZoomOut(){
    return levelOfdetails() > 0.015625;
}
