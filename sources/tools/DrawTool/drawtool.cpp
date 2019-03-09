#include "drawtool.h"
#include "ui_drawtool.h"
#include "global.h"
#include "adddrawitemundocommand.h"
#include <QDebug>
#include "items/drawitem.h"
#include "items/trackitem/angle.h"
#include "editdrawundocommand.h"

DrawTool::DrawTool(QWidget *parent, DrawItem *drawItem)
    : MYToolBase(parent),ui(new Ui::DrawTool){
    ui->setupUi(this);
    setToolName(tr("Draw"));
    setToolIcon(QIcon(":/pictures/draw_polyline.png"));
    m_cursor = Qt::CrossCursor;
    setAcceptRubberband(false);
    setAcceptAutoExpand(true,MYToolBase::Auto2);
    ui->widthSpinBox->setRawValue(25);
    ui->groundPlaneOffsetSpinBox->setRawValue(50);

    // fill the table with widgets (hidden in page2 of stacked widget)
    ui->tableWidget->setCellWidget(0,1,ui->nameLineEdit);
    ui->tableWidget->setCellWidget(1,1,ui->typeComboBox);
    ui->tableWidget->setCellWidget(2,1,ui->widthSpinBox);
    ui->tableWidget->setCellWidget(3,1,ui->fillTypeComboBox);
    ui->tableWidget->setCellWidget(4,1,ui->groundPlaneOffsetSpinBox);

    if ( drawItem == 0 ){
        ui->tableWidget->setRowHidden(0,true);
        m_drawItem = new DrawItem(0);
        m_drawItem->setToolItem(true);
        m_isDrawing = false;
        updateDrawType(0);
        updateWidth();
    }else{
        ui->tableWidget->setRowHidden(1,true);
        m_drawItem = drawItem;
        updateTool();
    }

    m_drawItem->setTool(this);

    connect(ui->widthSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateWidth()));
    connect(ui->groundPlaneOffsetSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateOffset()));
    connect(ui->typeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateDrawType(int)));
    connect(ui->fillTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateFillType(int)));
}

DrawTool::~DrawTool(){
    delete ui;
    m_drawItem->setTool(0);
    if ( m_drawItem->isToolItem() && ! m_drawItem->parentItem() )
        delete m_drawItem;
}

// udpdate widget values with texItem values
// disconnect and reconnect signals to avoid looping
void DrawTool::updateTool(){
    disconnect(ui->widthSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateWidth()));
    disconnect(ui->groundPlaneOffsetSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateOffset()));
    disconnect(ui->typeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateDrawType(int)));
    disconnect(ui->fillTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateFillType(int)));
    disconnect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged()));

    if ( ui->widthSpinBox->rawValue() != m_drawItem->width() )
        ui->widthSpinBox->setRawValue(m_drawItem->width());
    if ( ui->groundPlaneOffsetSpinBox->rawValue() != m_drawItem->groundPlaneOffset() )
        ui->groundPlaneOffsetSpinBox->setRawValue(m_drawItem->groundPlaneOffset());
    ui->typeComboBox->setCurrentIndex((int) m_drawItem->drawType());
    ui->fillTypeComboBox->setCurrentIndex((int) m_drawItem->fillType());
    if ( ui->fillTypeComboBox->currentIndex() == 2)
        ui->groundPlaneOffsetSpinBox->setEnabled(true);
    else
        ui->groundPlaneOffsetSpinBox->setEnabled(false);
    ui->nameLineEdit->setText(m_drawItem->itemName());
    emit nameChanged(m_drawItem);

    connect(ui->widthSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateWidth()));
    connect(ui->groundPlaneOffsetSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateOffset()));
    connect(ui->typeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateDrawType(int)));
    connect(ui->fillTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateFillType(int)));
    connect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged()));
}


void DrawTool::updateWidth(){
    // if drawing, update draw width
    if ( m_drawItem->isToolItem() ){
        m_drawItem->setWidth(ui->widthSpinBox->rawValue());
    }else{
        // we are editing width of an existing drawing
        EditDrawUndoCommand *drawEditCommand = new EditDrawUndoCommand(m_drawItem,
                                                                       ui->nameLineEdit->text(),
                                                                       ui->widthSpinBox->rawValue(),
                                                                       (DrawItem::FillType) ui->fillTypeComboBox->currentIndex(),
                                                                       m_drawItem->groundPlaneOffset());
        qApp->undoGroup()->activeStack()->push(drawEditCommand);
    }
}

void DrawTool::updateOffset(){
    if ( m_drawItem->isToolItem() ){
        m_drawItem->setGroundPlaneOffset(ui->groundPlaneOffsetSpinBox->rawValue()*2.0);
    }else{
        // we are editing width of an existing drawing
        EditDrawUndoCommand *drawEditCommand = new EditDrawUndoCommand(m_drawItem,
                                                                       ui->nameLineEdit->text(),
                                                                       ui->widthSpinBox->rawValue(),
                                                                       (DrawItem::FillType) ui->fillTypeComboBox->currentIndex(),
                                                                       m_drawItem->groundPlaneOffset());
        qApp->undoGroup()->activeStack()->push(drawEditCommand);
    }
}

void DrawTool::updateDrawType(int index){
    m_drawItem->setDrawType((DrawItem::DrawType) index);
    if ( index == 0 ){
        m_drawItem->clearTrack();
        for(int i =0; i < 4; i++)
            m_drawItem->appendAngle(QPointF(0,0),false);
    }
    if ( index == 1 ){
        m_drawItem->clearTrack();
        for(int i =0; i < 2; i++)
            m_drawItem->appendAngle(QPointF(0,0),false);
    }
    if ( index == 2 ){
        m_drawItem->clearTrack();
        for(int i =0; i < 4; i++)
            m_drawItem->appendAngle(QPointF(0,0),false);
    }
}

void DrawTool::updateFillType(int index){
    if ( m_drawItem->isToolItem() ){
        m_drawItem->setFillType((DrawItem::FillType) index);
        if (index == 2)
            ui->groundPlaneOffsetSpinBox->setEnabled(true);
        else
            ui->groundPlaneOffsetSpinBox->setEnabled(false);
    }else{
        // we are editing width of an existing drawing
        EditDrawUndoCommand *drawEditCommand = new EditDrawUndoCommand(m_drawItem,
                                                                       ui->nameLineEdit->text(),
                                                                       ui->widthSpinBox->rawValue(),
                                                                       (DrawItem::FillType) ui->fillTypeComboBox->currentIndex(),
                                                                       m_drawItem->groundPlaneOffset());
        qApp->undoGroup()->activeStack()->push(drawEditCommand);
    }
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_drawItem->groupBoundingRect());
}

void DrawTool::stopDrawing(){
    qApp->currentTypon()->typonScene()->removeTyponItem(m_drawItem);
    m_isDrawing = false;
}

void DrawTool::handleMousePressEvent(QMouseEvent *event){
    if ( event->button() == Qt::LeftButton ){
        if ( ! isEnabled() )
            return;
        if ( ! qApp->currentTypon()->typonScene()->sceneRect().contains(qApp->currentTypon()->typonView()->mapToScene(event->pos())) )
            return;
        if ( ! m_isDrawing ){
            m_isDrawing = true;
            m_drawItem->setParentItem(qApp->currentTypon()->typonScene()->layers()->activeLayer());
            for(int i =0; i < m_drawItem->angles().count(); i++){
                    m_drawItem->updateAnglePos(m_drawItem->angles().at(i), qApp->currentTypon()->typonView()->nearestGridPoint(
                           qApp->currentTypon()->typonView()->mapToScene(event->pos())));
            }
        }else{
            if ( m_drawItem->drawType() == DrawItem::Rectangle || m_drawItem->drawType() == DrawItem::Ellipse ){
                if ( m_drawItem->angles().at(0)->pos() != m_drawItem->angles().at(3)->pos() ){
                    AddDrawItemUndoCommand *addDrawItemCommand = new AddDrawItemUndoCommand(m_drawItem);
                    qApp->undoGroup()->activeStack()->push(addDrawItemCommand);
                    stopDrawing();
                }
            }
            if ( m_drawItem->drawType() == DrawItem::Polygon ){
                m_drawItem->appendAngle(m_drawItem->angles().last()->pos(),false);
                // modify Auto2 expand mode so it continue to expand even if a left clic occured
                m_isReadyToExpand = false;
            }
        }
    }
    if ( event->button() == Qt::RightButton ){
        if ( ! qApp->currentTypon()->typonScene()->sceneRect().contains(qApp->currentTypon()->typonView()->mapToScene(event->pos())) )
            return;
        if ( m_isDrawing ){
            if ( m_drawItem->drawType() == DrawItem::Rectangle || m_drawItem->drawType() == DrawItem::Ellipse )
                stopDrawing();
            if ( m_drawItem->drawType() == DrawItem::Polygon ){
                if ( m_drawItem->angles().at(0)->pos() != m_drawItem->angles().at(1)->pos() ){
                    AddDrawItemUndoCommand *addDrawItemCommand = new AddDrawItemUndoCommand(m_drawItem);
                    qApp->undoGroup()->activeStack()->push(addDrawItemCommand);
                    m_drawItem->clearTrack();
                    m_drawItem->appendAngle(QPointF(0,0),false);
                    m_drawItem->appendAngle(QPointF(0,0),false);
                    stopDrawing();
                }
            }
        }
    }

    MYToolBase::mousePressEventTool(event);
}

void DrawTool::handleMouseReleaseEvent(QMouseEvent *event){
    if ( event->button() == Qt::LeftButton || event->button() == Qt::MidButton )
        MYToolBase::mouseReleaseEventTool(event);
}

void DrawTool::handleMouseMoveEvent(QMouseEvent *event){
    QPointF posScene = qApp->currentTypon()->typonView()->nearestGridPoint(
                    qApp->currentTypon()->typonView()->mapToScene(event->pos()));
    if ( m_isDrawing ) {
        if ( m_drawItem->drawType() == DrawItem::Rectangle || m_drawItem->drawType() == DrawItem::Ellipse ){
            m_drawItem->updateAnglePos(m_drawItem->angles().at(2),QPointF(posScene.x(),posScene.y()));
        }
        if ( m_drawItem->drawType() == DrawItem::Polygon ){
            m_drawItem->updateAnglePos(m_drawItem->angles().last(),QPointF(posScene.x(),posScene.y()));
        }
    }
    MYToolBase::mouseMoveEventTool(event);
}

void DrawTool::handleWheelEvent(QWheelEvent *event){
    MYToolBase::mouseWheelEventTool(event);
    QPointF posScene = qApp->currentTypon()->typonView()->nearestGridPoint(
                    qApp->currentTypon()->typonView()->mapToScene(event->pos()));
    if ( m_isDrawing ) {
        if ( m_drawItem->drawType() == DrawItem::Rectangle || m_drawItem->drawType() == DrawItem::Ellipse ){
            m_drawItem->updateAnglePos(m_drawItem->angles().at(1),QPointF(posScene.x(),m_drawItem->angles().at(0)->groupPos().y()));
            m_drawItem->updateAnglePos(m_drawItem->angles().at(2),QPointF(posScene.x(),posScene.y()));
            m_drawItem->updateAnglePos(m_drawItem->angles().at(3),QPointF(m_drawItem->angles().at(0)->groupPos().x(),posScene.y()));
        }
        if ( m_drawItem->drawType() == DrawItem::Polygon ){
            m_drawItem->updateAnglePos(m_drawItem->angles().last(),QPointF(posScene.x(),posScene.y()));
        }
    }
}

void DrawTool::handleKeyPressEvent(QKeyEvent *event){
    if ( isEnabled() ){
        if (event->key() == Qt::Key_W ) {
            if ( event->modifiers().testFlag(Qt::ShiftModifier) )
                ui->widthSpinBox->stepUp();
            else
                ui->widthSpinBox->stepDown();
        }
        if (event->key() == Qt::Key_Escape ) {
            if ( m_isDrawing )
                stopDrawing();
        }
    }
    MYToolBase::keyPressEventTool(event);
}

void DrawTool::handleKeyReleaseEvent(QKeyEvent *event){
    MYToolBase::keyReleaseEventTool(event);
}

void DrawTool::handleEnterEvent(QEvent *event){
    MYToolBase::enterEventTool(event);
}

void DrawTool::handleLeaveEvent(QEvent *event){
    MYToolBase::leaveEventTool(event);
}

// change name value, can only occured on an existing item on the scene
void DrawTool::onNameChanged(){
    EditDrawUndoCommand *drawEditCommand = new EditDrawUndoCommand(m_drawItem,
                                                                   ui->nameLineEdit->text(),
                                                                   ui->widthSpinBox->rawValue(),
                                                                   (DrawItem::FillType) ui->fillTypeComboBox->currentIndex(),
                                                                   m_drawItem->groundPlaneOffset());
    qApp->undoGroup()->activeStack()->push(drawEditCommand);
    emit nameChanged(m_drawItem);
}
