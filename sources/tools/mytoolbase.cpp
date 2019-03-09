#include "mytoolbase.h"
#include "ui_mytoolbase.h"
#include "global.h"
#include <QDebug>
#include <QScrollBar>
#include "MoveTool/movetool.h"

MYToolBase::MYToolBase(QWidget *parent)
    : QWidget(parent), ui(new Ui::MYToolBase){
    ui->setupUi(this);
    m_cursor = Qt::ArrowCursor;
    // path used by tools to draw on the scene
    // ie: rubberband or custom path like in measure tool
    m_path = QPainterPath();
    // mid tool is the tool attributed at the mid button of the mouse: used to move around the scene
    m_midMoveTool = new MoveToolCore(this,&m_cursor);
    m_ctrlKeyIsPressed = false;
    m_acceptWheelEvents = true;
    m_acceptRubberBand = false;
    m_acceptAutoExpand = false;
    m_autoExpandMode = Normal;
    m_isRubberBanding = false;
    m_needToBeDrawn = false;
    m_isReadyToExpand = false;
    m_rubberBandRect = QRectF();
    isExtending = false;
    m_extendingTimer = new QTimer(this);
    connect(m_extendingTimer, SIGNAL(timeout()), this, SLOT(extend()));
}

MYToolBase::~MYToolBase(){
    delete ui;
}

QCursor MYToolBase::cursor(){
    return m_cursor;
}

// plan redraw for the bounding rect (only sides are redrawn, not all the rect surface
void MYToolBase::invalidateRect(QRectF r){
    qApp->currentTypon()->typonScene()->invalidate(QRectF(r.left() ,r.top(),1,r.height()).adjusted(-0.5,-0.5,0.5,0.5),QGraphicsScene::AllLayers);
    qApp->currentTypon()->typonScene()->invalidate(QRectF(r.right() ,r.top(),1,r.height()).adjusted(-0.5,-0.5,0.5,0.5),QGraphicsScene::AllLayers);
    qApp->currentTypon()->typonScene()->invalidate(QRectF(r.left() ,r.top(),r.width(),1).adjusted(-0.5,-0.5,0.5,0.5),QGraphicsScene::AllLayers);
    qApp->currentTypon()->typonScene()->invalidate(QRectF(r.left() ,r.bottom(),r.width(),1).adjusted(-0.5,-0.5,0.5,0.5),QGraphicsScene::AllLayers);
}

void MYToolBase::setToolName(QString name){
    m_toolName = name;
}

QString MYToolBase::toolName(){
    return m_toolName;
}

void MYToolBase::setToolIcon(QIcon icon){
    m_icon = icon;
}

QIcon MYToolBase::toolIcon(){
    return m_icon;
}

void MYToolBase::setAcceptWheelEvents(bool accept){
    m_acceptWheelEvents = accept;
}

bool MYToolBase::acceptWheelEvents(){
    return m_acceptWheelEvents;
}

void MYToolBase::setAcceptRubberband(bool accept){
    m_acceptRubberBand = accept;
}

bool MYToolBase::acceptRubberBand(){
    return m_acceptRubberBand;
}

void MYToolBase::setAcceptAutoExpand(bool accept, AutoExpandMode mode){
    m_acceptAutoExpand = accept;
    m_autoExpandMode = mode;
}

bool MYToolBase::acceptAutoExpand(){
    return m_acceptAutoExpand;
}

QPainterPath MYToolBase::path(){
    return m_path;
}

void MYToolBase::mousePressEventTool(QMouseEvent *event){
    event->ignore();
    // handle left click
    if ( event->button() == Qt::LeftButton ){
        // update last mouse pos
        m_lastPosScene = qApp->currentTypon()->typonView()->mapToScene(event->pos().x(),event->pos().y());
        // if the current tool manage rubberband: start a new rubberband with empty size
        if ( m_acceptRubberBand ) {
            m_isRubberBanding = true;
            m_rubberBandRect.setTopLeft(m_lastPosScene);
            m_rubberBandRect.setSize(QSize(0,0));
        }
        // if the current tool accept auto expand
        if ( m_acceptAutoExpand ){
            if ( m_autoExpandMode == Auto2 ){
                // if mode is Auto2 and we are ready to expand a new left clic will stop the capacity to expand
                if ( m_isReadyToExpand )
                    stopExtending();
                m_isReadyToExpand = !m_isReadyToExpand;
            }else{
                // we are ready to expand if mouse go out of viewport
                m_isReadyToExpand = true;
            }
        }
        if ( m_isReadyToExpand )
            qApp->currentTypon()->typonView()->viewport()->grabMouse();
        else
            qApp->currentTypon()->typonView()->viewport()->releaseMouse();
    }
    // handle mid button
    if ( event->button() == Qt::MidButton ){
        m_midMoveTool->handleMousePressEvent(event);
    }

    // if right button is pressed
    if ( event->button() == Qt::RightButton ){
        // if we are expanding and mode is waiting to a new clic to stop then stop expanding and release mouse
        if ( m_isReadyToExpand == true && (m_autoExpandMode == Auto || m_autoExpandMode == Auto2)){
            m_isReadyToExpand = false;
            qApp->currentTypon()->typonView()->viewport()->releaseMouse();
            stopExtending();
        }
    }
}

void MYToolBase::mouseReleaseEventTool(QMouseEvent *event){
    event->ignore();
    if ( event->button() == Qt::LeftButton ){
        // if we were expandnig in Normal mode, stop
        if ( m_autoExpandMode == Normal ){
            m_isReadyToExpand = false;
            qApp->currentTypon()->typonView()->viewport()->releaseMouse();
            stopExtending();
        }
        // stop rubberbanding
        m_isRubberBanding = false;
        invalidateRect(m_rubberBandRect.normalized());
        m_rubberBandRect = QRectF(0,0,-1,-1);
    }
    // stop moving if the mid mouse button has been released
    if ( event->button() == Qt::MidButton ){
        m_midMoveTool->handleMouseReleaseEvent(event);
    }
}

void MYToolBase::mouseMoveEventTool(QMouseEvent *event){
    // if we are rubberbanding, update rect geometry
    if ( m_isRubberBanding ){
        m_lastPosScene = qApp->currentTypon()->typonView()->mapToScene(event->pos().x(),event->pos().y());
        invalidateRect(m_rubberBandRect.normalized());
        m_rubberBandRect.setBottomRight(m_lastPosScene);
        invalidateRect(m_rubberBandRect.normalized());
    }
    // if we are ready to expand
    if ( m_isReadyToExpand ){
        // start or stop actual autoScroll wether mouse cursor is or isn't inside viewport
        if ( qApp->currentTypon()->typonView()->viewport()->rect().contains(event->pos()) ){
            qApp->currentTypon()->typonView()->viewport()->releaseMouse();
            stopExtending();
        }else{
            startExtending();
        }
    }
    // handle mouse move with mid button
    m_midMoveTool->handleMouseMoveEvent(event);
}

void MYToolBase::mouseWheelEventTool(QWheelEvent *event){
    // update rubber band if we are rubberbanding
    if ( m_isRubberBanding ){
        m_lastPosScene = qApp->currentTypon()->typonView()->mapToScene(event->pos().x(),event->pos().y());
        invalidateRect(m_rubberBandRect.normalized());
        m_rubberBandRect.setBottomRight(m_lastPosScene);
        invalidateRect(m_rubberBandRect.normalized());
    }

    // If mouse wheel is used to zoom the scene, caculate wheel direction, then zoom in or out accordingly
    bool in = event->delta() > 0 ? true : false;
    if ( midZoomModOn() )
        zoomWheel(in);
}

void MYToolBase::zoomWheel(bool in){
    qApp->currentTypon()->typonView()->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    if ( in )
        qApp->currentTypon()->zoomIn();
    else
        qApp->currentTypon()->zoomOut();
}

void MYToolBase::keyPressEventTool(QKeyEvent *event){
    // control key is pressed => Disable grid snapping temporarly
    if ( event->key() == Qt::Key_Control ){
        qApp->currentTypon()->typonView()->momentaryDisableSnap(true);
        m_ctrlKeyIsPressed = true;
    }
    // if escape key is pressed and autoexpand is activated whith auto mode, stop autoexpanding
    if ( event->key() == Qt::Key_Escape ){
        if ( m_isReadyToExpand == true && (m_autoExpandMode == Auto || m_autoExpandMode == Auto2)){
            m_isReadyToExpand = false;
            qApp->currentTypon()->typonView()->viewport()->releaseMouse();
            stopExtending();
        }
    }
}

void MYToolBase::keyReleaseEventTool(QKeyEvent *event){
    // ctrl key is released, let snap to grid again (accordingly to general options)
    if ( event->key() == Qt::Key_Control ){
        qApp->currentTypon()->typonView()->momentaryDisableSnap(false);
        m_ctrlKeyIsPressed = false;
    }
}

// ctrl key swap mouse wheel beahviour: if default beahviour is zoom, ctrl key will allow moving on the scene with wheel
// and the opposite, get if we are zooming or not if wheel event occure
bool MYToolBase::midZoomModOn(){
    return !qApp->workSpace()->zoomOnWheel != !m_ctrlKeyIsPressed;
}

// used by the view to know if it needs to draw the rubberband
bool MYToolBase::needToDrawRubberBand(){
    return m_isRubberBanding;
}

// used by the view to know if the current tool has a path to draw
bool MYToolBase::needToBeDrawn(){
    return m_needToBeDrawn;
}

QRectF MYToolBase::rubberBandRect(){
    return m_rubberBandRect.normalized();
}

// call on timer timeout, scroll scene by a prefifined step
void MYToolBase::extend(){
    // find wich direction to expand
    int xExtend = 0;
    int yExtend = 0;
    if ( qApp->currentTypon()->typonView()->lastMousePos().x() > qApp->currentTypon()->typonView()->viewport()->rect().right() )
        xExtend = 1;
    if ( qApp->currentTypon()->typonView()->lastMousePos().x() < qApp->currentTypon()->typonView()->viewport()->rect().left() )
        xExtend = -1;
    if ( qApp->currentTypon()->typonView()->lastMousePos().y() > qApp->currentTypon()->typonView()->viewport()->rect().bottom() )
        yExtend = 1;
    if ( qApp->currentTypon()->typonView()->lastMousePos().y() < qApp->currentTypon()->typonView()->viewport()->rect().top() )
        yExtend = -1;

    int extendLenght = 6;
    if ( xExtend !=0 && yExtend != 0 ) //expnding in both directions, go slower
        extendLenght = 3;

    // set scrollbar with the new position
    QScrollBar *hBar = qApp->currentTypon()->typonView()->horizontalScrollBar();
    QScrollBar *vBar = qApp->currentTypon()->typonView()->verticalScrollBar();
    hBar->setValue(hBar->value() + xExtend * extendLenght);
    vBar->setValue(vBar->value() + yExtend * extendLenght);

    // generate a mouse move event to force redraw of lines, selectionrect and rulers values
    QCursor cursor;
    QPoint point = qApp->currentTypon()->typonView()->mapFromGlobal(cursor.pos());
    QMouseEvent e(QEvent::MouseMove,point,Qt::NoButton,Qt::NoButton,Qt::NoModifier);
    handleMouseMoveEvent(&e);
    qApp->currentTypon()->typonView()->updateLastMousePos(true);
}

void MYToolBase::startExtending(){
    if ( isExtending )
        return;
    isExtending = true;
    m_extendingTimer->start(10);
}

void MYToolBase::stopExtending(){
    isExtending = false;
    m_extendingTimer->stop();
}

void MYToolBase::enterEventTool(QEvent *event){
    Q_UNUSED(event)
    m_ctrlKeyIsPressed = QApplication::keyboardModifiers().testFlag(Qt::ControlModifier);
}

void MYToolBase::leaveEventTool(QEvent *event){
    Q_UNUSED(event)
    // we leave viewport, if we are ready to extand, let extand
    if ( m_isReadyToExpand ){
        startExtending();
    }
}
