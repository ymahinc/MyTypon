#include "mytyponscrollbar.h"
#include <QDebug>

MYTyponScrollBar::MYTyponScrollBar(QWidget * parent)
    : QScrollBar(parent) {
    setMouseTracking(true);
    m_view = qobject_cast<MYTyponView *>(parent);
    setCursor(Qt::ArrowCursor);
}

// used to hide lines under mouse cursor when quitting the actual view aera
void MYTyponScrollBar::mouseMoveEvent(QMouseEvent * e){
    QScrollBar::mouseMoveEvent(e);
    m_view->leaveViewport();
}
