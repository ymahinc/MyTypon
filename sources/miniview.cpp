#include "miniview.h"
#include "global.h"
#include <QDebug>

MiniView::MiniView(QWidget *parent)
    : QGraphicsView(parent){
    init();
}

MiniView::MiniView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent){
    init();
}

void MiniView::init(){
    m_isMoving = false;
    setViewportUpdateMode(FullViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing,true);
    setMouseTracking(true);
    setInteractive(false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(ScrollHandDrag);
}

//draw a black background, the board rect and a white rect that represent the viewport of the current typon
void MiniView::drawBackground(QPainter* painter, const QRectF &rect){
    painter->setClipRect(sceneRect());
    painter->fillRect(rect,Qt::black);
    QPen pen(Qt::yellow);
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->drawRect(qApp->currentTypon()->typonView()->typonRect());
    pen.setColor(Qt::white);
    painter->setPen(pen);
    painter->drawRect(qApp->currentTypon()->typonView()->sceneViewportRect());
}

// current typon has changed, update the scene
void MiniView::updateTypon(){
    setScene(qApp->currentTypon()->typonScene());
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}

void MiniView::resizeEvent(QResizeEvent*){
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}

void MiniView::mousePressEvent(QMouseEvent* event){
    Q_UNUSED(event);
    viewport()->setCursor(Qt::ClosedHandCursor);
    m_isMoving = true;
    qApp->currentTypon()->typonView()->centerOn(mapToScene(event->pos()));
    scene()->invalidate();
}

void MiniView::mouseReleaseEvent(QMouseEvent* event){
    Q_UNUSED(event);
    viewport()->setCursor(Qt::OpenHandCursor);
    m_isMoving = false;
}

// if mouse move when mouse buttons are clicked, center the current typon on this position
void MiniView::mouseMoveEvent(QMouseEvent* event){
    if ( m_isMoving ){
        qApp->currentTypon()->typonView()->centerOn(mapToScene(event->pos()));
        scene()->invalidate();
    }
}

void MiniView::invalidate(){
    scene()->invalidate();
}
