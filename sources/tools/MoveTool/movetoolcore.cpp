#include "movetoolcore.h"
#include <QScrollBar>
#include "global.h"
#include <QDebug>

MoveToolCore::MoveToolCore(QObject * parent, QCursor *oldCursor)
    : QObject(parent), m_oldCursor(oldCursor) {
    m_isDragging = false;
}

void MoveToolCore::handleMousePressEvent(QMouseEvent *event){
    Q_UNUSED(event)
    qApp->currentTypon()->typonView()->setCursor(Qt::ClosedHandCursor);
    m_isDragging = true;
}

void MoveToolCore::handleMouseReleaseEvent(QMouseEvent *event){
    Q_UNUSED(event)
    qApp->currentTypon()->typonView()->setCursor(*m_oldCursor);
    m_isDragging = false;
}

void MoveToolCore::handleMouseMoveEvent(QMouseEvent *event){
    // update scrollbar values to drag the view
    if ( m_isDragging ) {
        QScrollBar *hBar = qApp->currentTypon()->typonView()->horizontalScrollBar();
        QScrollBar *vBar = qApp->currentTypon()->typonView()->verticalScrollBar();
        QPoint delta = event->pos() - qApp->currentTypon()->typonView()->lastMousePos();
        hBar->setValue(hBar->value() - delta.x());
        vBar->setValue(vBar->value() - delta.y());
        qApp->currentTypon()->typonScene()->invalidate();
    }
}

void MoveToolCore::setDragging(bool on){
    m_isDragging = on;
}

bool MoveToolCore::isDragging(){
    return m_isDragging;
}
