#include "measuretool.h"
#include "ui_measuretool.h"
#include "global.h"
#include <QtMath>
#include <QDebug>

MeasureTool::MeasureTool(QWidget *parent)
    : MYToolBase(parent), ui(new Ui::MeasureTool)
{
    ui->setupUi(this);
    setAcceptRubberband(false);
    setAcceptAutoExpand(true);
    setToolName(tr("Ruler"));
    setToolIcon(QIcon(":/pictures/regle.png"));
    m_cursor = QCursor(QPixmap(":/pictures/reglecur.png"),5,5);
    ui->widthSpinBox->setAllowZero(true);
    ui->heightSpinBox->setAllowZero(true);
    ui->lenghtSpinBox->setAllowZero(true);
    ui->widthSpinBox->setRawValue(0.0);
    ui->heightSpinBox->setRawValue(0.0);
    ui->lenghtSpinBox->setRawValue(0.0);
    ui->angleSpinBox->setValue(0.0);

    // fill the table with widgets (hidden in page2 of stacked widget)
    ui->tableWidget->setCellWidget(0,1,ui->widthSpinBox);
    ui->tableWidget->setCellWidget(1,1,ui->heightSpinBox);
    ui->tableWidget->setCellWidget(2,1,ui->lenghtSpinBox);
    ui->tableWidget->setCellWidget(3,1,ui->angleSpinBox);

    m_isMeasuring = false;
}

MeasureTool::~MeasureTool(){
    delete ui;
}

void MeasureTool::handleMousePressEvent(QMouseEvent *event){
    // start measuring if we clic inside the scene rect
    if ( event->button() == Qt::LeftButton ){
        if ( ! qApp->currentTypon()->typonScene()->sceneRect().contains(qApp->currentTypon()->typonView()->mapToScene(event->pos())) )
            return;
        m_isMeasuring = true;
        m_needToBeDrawn = true;
        // update start and last point to establish distances and angle
        m_firstPoint = qApp->currentTypon()->typonView()->nearestGridPoint(
                    qApp->currentTypon()->typonView()->mapToScene(event->pos()));
        m_secondPoint = m_firstPoint;
        // call default tool behaviour in order to handle expansion
        MYToolBase::mousePressEventTool(event);
    }
    // call default behaviour for mid button
    if ( event->button() == Qt::MidButton )
        MYToolBase::mousePressEventTool(event);
}

void MeasureTool::handleMouseReleaseEvent(QMouseEvent *event){
    // stop measuring, clear all widgets, clear path and update scene
    if ( event->button() == Qt::LeftButton ){
        m_isMeasuring = false;
        m_needToBeDrawn = false;
        ui->widthSpinBox->setRawValue(0.0);
        ui->heightSpinBox->setRawValue(0.0);
        ui->lenghtSpinBox->setRawValue(0.0);
        ui->angleSpinBox->setValue(0.0);
        MYToolBase::mouseReleaseEventTool(event);
        updateBoundingRect();
        m_path = QPainterPath();
    }
    if ( event->button() == Qt::MidButton )
        MYToolBase::mouseReleaseEventTool(event);
}

void MeasureTool::handleMouseMoveEvent(QMouseEvent *event){
    // update path and widgets values while measuring
    if ( m_isMeasuring ){
        updateBoundingRect();
        updatePath(event->pos());
        updateBoundingRect();
        updateWidgets();
    }
    // call default behaviour to handle expanding
    MYToolBase::mouseMoveEventTool(event);
}

// compute widgets value
void MeasureTool::updateWidgets(){
    QPointF lenghtVector = m_secondPoint - m_firstPoint;
    ui->widthSpinBox->setRawValue(qFabs(lenghtVector.x()));
    ui->heightSpinBox->setRawValue(qFabs(lenghtVector.y()));
    ui->lenghtSpinBox->setRawValue(qFabs(QLineF(QPointF(0,0),lenghtVector).length()));
    ui->angleSpinBox->setValue(QLineF(m_firstPoint,m_secondPoint).angle());
}

void MeasureTool::updatePath(QPoint point){
    // update path according to new point
    m_secondPoint = qApp->currentTypon()->typonView()->nearestGridPoint(
                qApp->currentTypon()->typonView()->mapToScene(point));
    // clean the path
    m_path = QPainterPath();
    // start 100 on the right of start point in order to draw the "base" axis
    QPoint startPoint = qApp->currentTypon()->typonView()->mapFromScene(m_firstPoint) + QPoint(100,0);
    QPointF mappedstartPoint = qApp->currentTypon()->typonView()->mapToScene(startPoint);
    m_path.moveTo(mappedstartPoint);
    m_path.lineTo(m_firstPoint);
    //from the start point, draw a line to the second point (under mouse cursor)
    m_path.lineTo(m_secondPoint);

    // if the line lenght is sufficient, draw an arc in order to represent the angle
    QLineF line = QLineF( qApp->currentTypon()->typonView()->mapFromScene(m_firstPoint)
                        , qApp->currentTypon()->typonView()->mapFromScene(m_secondPoint));

    if ( qRound(line.length()) >= 25 ) {
        double angle = line.angle();
        QPoint topLeft = qApp->currentTypon()->typonView()->mapFromScene(m_firstPoint) + QPoint(-25,-25);
        QPoint bottomRight = qApp->currentTypon()->typonView()->mapFromScene(m_firstPoint) + QPoint(25,25);
        QRectF r = QRectF(qApp->currentTypon()->typonView()->mapToScene(topLeft),qApp->currentTypon()->typonView()->mapToScene(bottomRight));
        m_path.moveTo(m_firstPoint);
        m_path.arcTo(r, 0, angle);
    }
}

void MeasureTool::updateBoundingRect(){
    qApp->currentTypon()->typonScene()->invalidate(m_path.boundingRect().normalized(),QGraphicsScene::AllLayers);
}

void MeasureTool::handleWheelEvent(QWheelEvent *event){
    // call default behaviour
    MYToolBase::mouseWheelEventTool(event);
    // if measuring update path and values
    if ( m_isMeasuring ){
        updateBoundingRect();
        updatePath(event->pos());
        updateBoundingRect();
        updateWidgets();
    }
}

void MeasureTool::handleKeyPressEvent(QKeyEvent *event){
    MYToolBase::keyPressEventTool(event);
}

void MeasureTool::handleKeyReleaseEvent(QKeyEvent *event){
    MYToolBase::keyReleaseEventTool(event);
}

void MeasureTool::handleEnterEvent(QEvent *event){
    MYToolBase::enterEventTool(event);
}

void MeasureTool::handleLeaveEvent(QEvent *event){
    MYToolBase::leaveEventTool(event);
}
