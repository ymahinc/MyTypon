#include "movetool.h"
#include "ui_movetool.h"

MoveTool::MoveTool(QWidget *parent)
    : MYToolBase(parent),ui(new Ui::MoveTool){
    ui->setupUi(this);
    setToolName(tr("Move"));
    setToolIcon(QIcon(":/pictures/transform_move16.png"));
    m_cursor = Qt::OpenHandCursor;
    // all stuff is done by the coreTool
    m_moveCore = new MoveToolCore(this,&m_cursor);
}

MoveTool::~MoveTool(){
    delete ui;
}

void MoveTool::handleMousePressEvent(QMouseEvent *event){
    if ( event->button() == Qt::LeftButton || event->button() == Qt::MidButton )
        m_moveCore->handleMousePressEvent(event);
}

void MoveTool::handleMouseReleaseEvent(QMouseEvent *event){
    if ( event->button() == Qt::LeftButton || event->button() == Qt::MidButton )
        m_moveCore->handleMouseReleaseEvent(event);
}

void MoveTool::handleMouseMoveEvent(QMouseEvent *event){
    m_moveCore->handleMouseMoveEvent(event);
}

void MoveTool::handleWheelEvent(QWheelEvent *event){
    MYToolBase::mouseWheelEventTool(event);
}

void MoveTool::handleKeyPressEvent(QKeyEvent *event){
    MYToolBase::keyPressEventTool(event);
}

void MoveTool::handleKeyReleaseEvent(QKeyEvent *event){
    MYToolBase::keyReleaseEventTool(event);
}

void MoveTool::handleEnterEvent(QEvent *event){
    MYToolBase::enterEventTool(event);
}

void MoveTool::handleLeaveEvent(QEvent *event){
    MYToolBase::leaveEventTool(event);
}
