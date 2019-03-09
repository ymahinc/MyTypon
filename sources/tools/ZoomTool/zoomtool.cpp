#include "zoomtool.h"
#include "ui_zoomtool.h"
#include "global.h"
#include <QScrollBar>
#include <QDebug>

ZoomTool::ZoomTool(QWidget *parent)
    : MYToolBase(parent), ui(new Ui::ZoomTool){
    ui->setupUi(this);
    m_isZoomingIn = true;
    setAcceptRubberband(true);
    setToolName(tr("Zoom"));
    setToolIcon(QIcon(":/pictures/view.png"));
    m_cursor = QCursor(QPixmap(":/pictures/zoom_incur.png"),0,0);
    connect(ui->zoomInPushButton,SIGNAL(clicked(bool)),this,SIGNAL(zoomIn()));
    connect(ui->zoomOutPushButton,SIGNAL(clicked(bool)),this,SIGNAL(zoomOut()));
    connect(ui->zoomBoardPushButton,SIGNAL(clicked(bool)),this,SIGNAL(zoomBoard()));
    connect(ui->zoomResetPushButton,SIGNAL(clicked(bool)),this,SIGNAL(zoomReset()));
    connect(ui->zoomFitPushButton,SIGNAL(clicked(bool)),this,SIGNAL(zoomFit()));
    connect(ui->zoomSelectionPushButton,SIGNAL(clicked(bool)),this,SIGNAL(zoomSelection()));
}

ZoomTool::~ZoomTool(){
    delete ui;
}

void ZoomTool::handleMousePressEvent(QMouseEvent *event){
    MYToolBase::mousePressEventTool(event);
    // disable wheel events when rubberbanding
    if ( event->button() == Qt::LeftButton )
        setAcceptWheelEvents(false);
}

void ZoomTool::handleMouseReleaseEvent(QMouseEvent *event){
    // mouse is released, let zoom the rubberband rect
    // in if ctrl key is released, out if it is pressed
    if ( event->button() == Qt::LeftButton )
        emit zoomRect(m_rubberBandRect,m_isZoomingIn);
    MYToolBase::mouseReleaseEventTool(event);
    // accept wheel event again
    setAcceptWheelEvents(true);
}

void ZoomTool::handleMouseMoveEvent(QMouseEvent *event){
    MYToolBase::mouseMoveEventTool(event);
}

void ZoomTool::handleWheelEvent(QWheelEvent *event){
    MYToolBase::mouseWheelEventTool(event);
}

void ZoomTool::handleKeyPressEvent(QKeyEvent *event){
    // if ctrl is pressed change zoom direction and update cursor
    if ( event->key() == Qt::Key_Control ){
        m_cursor = QCursor(QPixmap(":/pictures/zoom_outcur.png"),0,0);
        qApp->currentTypon()->typonView()->setCursor(m_cursor);
        m_isZoomingIn = false;
    }
    MYToolBase::keyPressEventTool(event);
}

void ZoomTool::handleKeyReleaseEvent(QKeyEvent *event){
    // if ctrl is released change zoom direction and update cursor
    if ( event->key() == Qt::Key_Control ){
        m_cursor = QCursor(QPixmap(":/pictures/zoom_incur.png"),0,0);
        qApp->currentTypon()->typonView()->setCursor(m_cursor);
        m_isZoomingIn = true;
    }
    MYToolBase::keyReleaseEventTool(event);
}

// enable zoom in or zoom out depending on current zoom
void ZoomTool::updateWidgets(){
    ui->zoomInPushButton->setEnabled(qApp->currentTypon()->typonView()->canZoomIn());
    ui->zoomOutPushButton->setEnabled(qApp->currentTypon()->typonView()->canZoomOut());
}

void ZoomTool::handleEnterEvent(QEvent *event){
    MYToolBase::enterEventTool(event);
}

void ZoomTool::handleLeaveEvent(QEvent *event){
    MYToolBase::leaveEventTool(event);
}
