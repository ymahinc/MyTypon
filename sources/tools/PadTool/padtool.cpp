#include "padtool.h"
#include "ui_padtool.h"
#include "global.h"
#include <QDebug>
#include "addpadundocommand.h"
#include "editpadundocommand.h"

PadTool::PadTool(QWidget *parent, PadGroup *item)
    : MYToolBase(parent), ui(new Ui::PadTool){
    ui->setupUi(this);
    setAcceptRubberband(false);
    setAcceptAutoExpand(false);
    setToolName(tr("Pad"));
    setToolIcon(QIcon(":/pictures/polyrectpad.png"));
    m_cursor = Qt::CrossCursor;

    // fill the table with widgets (hidden in page2 of stacked widget)
    ui->tableWidget->setCellWidget(0,1,ui->nameLineEdit);
    ui->tableWidget->setCellWidget(1,1,ui->typeComboBox);
    ui->tableWidget->setCellWidget(2,1,ui->widthSpinBox);
    ui->tableWidget->setCellWidget(3,1,ui->holeCheckBox);
    ui->tableWidget->setCellWidget(4,1,ui->holeSpinBox);
    ui->tableWidget->setCellWidget(5,1,ui->viaCheckBox);
    ui->tableWidget->setCellWidget(6,1,ui->busCheckBox);
    ui->tableWidget->setCellWidget(7,1,ui->busComboBox);
    ui->tableWidget->setCellWidget(8,1,ui->busSpinBox);
    ui->tableWidget->setCellWidget(9,1,ui->busSpacingSpinBox);

    if ( item == 0 ){
        // as it is a tool item hide name widget, we dont want to edit this now
        ui->tableWidget->setRowHidden(0,true);
        // set default values
        m_group = new PadGroup;
        m_group->setToolItem(true);

        connect(ui->busCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateBus()));
        connect(ui->busSpinBox,SIGNAL(valueChanged(int)),this,SLOT(updateBus()));
        connect(ui->typeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updatePadType(int)));
        connect(ui->holeCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateHole()));
        connect(ui->widthSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateWidth()));
        connect(ui->holeSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateHoleWidth()));
        connect(ui->viaCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateVia()));
        connect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged()));

        ui->typeComboBox->setCurrentIndex(5);
        ui->busComboBox->setCurrentIndex(1);
        ui->widthSpinBox->setRawValue(108);
        ui->holeCheckBox->setChecked(true);
        ui->holeSpinBox->setRawValue(35);
        ui->busSpacingSpinBox->setRawValue(qApp->workSpace()->grid->xStepGrid() * 2.0);
        m_holeWasActivated = true;
        updateBus();       
    }else{
        // as it is a seletec item hide bus relative staff
        ui->tableWidget->setRowHidden(6,true);
        ui->tableWidget->setRowHidden(7,true);
        ui->tableWidget->setRowHidden(8,true);
        ui->tableWidget->setRowHidden(9,true);

        m_group = item;
        updateTool();
    }

    // set a pointer to this on the item so we can update widgets values when item properties change
    m_group->setTool(this);
}

PadTool::~PadTool(){
    delete ui;
    // if m_group isToolItem, the tool is the application one, so we are quitting app, so delete item
    // else tool is one of those used in selection treeWidget, do not delete linked m_group

    if ( m_group->isToolItem() ) {
        m_group->removeFromGroup(m_group->pad());
        m_group->removeFromGroup(m_group->hole());
        if ( ! m_group->pad()->parentItem() )
            delete m_group->pad();
        if ( ! m_group->hole()->parentItem() )
            delete m_group->hole();
        clearBus();
        m_group->setTool(0);
        delete m_group;
    }else{
        m_group->setTool(0);
    }
}

void PadTool::updateTool(){
    disconnect(ui->busCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateBus()));
    disconnect(ui->busSpinBox,SIGNAL(valueChanged(int)),this,SLOT(updateBus()));
    disconnect(ui->typeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updatePadType(int)));
    disconnect(ui->holeCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateHole()));
    disconnect(ui->widthSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateWidth()));
    disconnect(ui->holeSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateHoleWidth()));
    disconnect(ui->viaCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateVia()));
    disconnect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged()));

    ui->nameLineEdit->setText(m_group->itemName());
    ui->typeComboBox->setCurrentIndex((int)m_group->pad()->padType());
    if ( ui->widthSpinBox->rawValue() != m_group->pad()->width() )
        ui->widthSpinBox->setRawValue(m_group->pad()->width());
    ui->holeCheckBox->setChecked(m_group->hasHole());
    m_holeWasActivated = m_group->hasHole();
    if ( ui->holeSpinBox->rawValue() != m_group->hole()->width() )
        ui->holeSpinBox->setRawValue(m_group->hole()->width());
    ui->viaCheckBox->setChecked(m_group->isVia());

    if ( !m_group->hasHole() ){
        ui->holeSpinBox->setEnabled(false);
        ui->viaCheckBox->setEnabled(false);
    }

    if (m_group->pad()->padType() == PadItem::Hole){
        ui->holeCheckBox->setChecked(true);
        ui->holeCheckBox->setEnabled(false);
    }

    emit nameChanged(m_group);

    connect(ui->busCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateBus()));
    connect(ui->busSpinBox,SIGNAL(valueChanged(int)),this,SLOT(updateBus()));
    connect(ui->typeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updatePadType(int)));
    connect(ui->holeCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateHole()));
    connect(ui->widthSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateWidth()));
    connect(ui->holeSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateHoleWidth()));
    connect(ui->viaCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateVia()));
    connect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged()));
}

void PadTool::updateVia(){
    if ( m_group->isToolItem() ){
        m_group->setVia(ui->viaCheckBox->isChecked());
    }else{
        EditPadUndoCommand *cmd = new EditPadUndoCommand(m_group,
                                                         ui->nameLineEdit->text(),
                                                         (PadItem::PadType)ui->typeComboBox->currentIndex(),
                                                         ui->widthSpinBox->rawValue(),
                                                         ui->holeCheckBox->isChecked(),
                                                         ui->holeSpinBox->rawValue(),
                                                         ui->viaCheckBox->isChecked());
        qApp->currentTypon()->undoStack()->push(cmd);
    }

    for ( int i = 0; i < m_bus.count(); i++ ){
        m_bus.at(i)->setVia(ui->viaCheckBox->isChecked());
        qApp->currentTypon()->typonScene()->updateGroundPlanes(m_group->boundingRect().translated(m_group->pos()));
    }
}

void PadTool::updateHole(){    
    if ( ui->holeCheckBox->isChecked() ){
        ui->holeSpinBox->setEnabled(true);
        ui->viaCheckBox->setEnabled(true);
    }else{
        ui->holeSpinBox->setEnabled(false);
        ui->viaCheckBox->setEnabled(false);
        ui->viaCheckBox->setChecked(false);
    }
    m_holeWasActivated = ui->holeCheckBox->isChecked();

    if ( m_group->isToolItem() ){
        m_group->setHasHole(ui->holeCheckBox->isChecked());
    }else{
        EditPadUndoCommand *cmd = new EditPadUndoCommand(m_group,
                                                         ui->nameLineEdit->text(),
                                                         (PadItem::PadType)ui->typeComboBox->currentIndex(),
                                                         ui->widthSpinBox->rawValue(),
                                                         ui->holeCheckBox->isChecked(),
                                                         ui->holeSpinBox->rawValue(),
                                                         ui->viaCheckBox->isChecked());
        qApp->currentTypon()->undoStack()->push(cmd);
    }

    for ( int i = 0; i < m_bus.count(); i++ )
        m_bus.at(i)->setHasHole(ui->holeCheckBox->isChecked());
}

void PadTool::updateWidth(){
    if ( m_group->isToolItem() ){
        m_group->pad()->setWidth(ui->widthSpinBox->rawValue());
        for ( int i = 0; i < m_bus.count(); i++ )
            m_bus.at(i)->pad()->setWidth(ui->widthSpinBox->rawValue());
    }else{
        EditPadUndoCommand *cmd = new EditPadUndoCommand(m_group,
                                                         ui->nameLineEdit->text(),
                                                         (PadItem::PadType)ui->typeComboBox->currentIndex(),
                                                         ui->widthSpinBox->rawValue(),
                                                         ui->holeCheckBox->isChecked(),
                                                         ui->holeSpinBox->rawValue(),
                                                         ui->viaCheckBox->isChecked());
        qApp->currentTypon()->undoStack()->push(cmd);
    }
}

void PadTool::updateHoleWidth(){
    if ( m_group->isToolItem() ){
        m_group->setHoleWidth(ui->holeSpinBox->rawValue());
        for ( int i = 0; i < m_bus.count(); i++ )
            m_bus.at(i)->setHoleWidth(ui->holeSpinBox->rawValue());
    }else{
        EditPadUndoCommand *cmd = new EditPadUndoCommand(m_group,
                                                         ui->nameLineEdit->text(),
                                                         (PadItem::PadType)ui->typeComboBox->currentIndex(),
                                                         ui->widthSpinBox->rawValue(),
                                                         ui->holeCheckBox->isChecked(),
                                                         ui->holeSpinBox->rawValue(),
                                                         ui->viaCheckBox->isChecked());
        qApp->currentTypon()->undoStack()->push(cmd);
    }
}

void PadTool::updatePadType(int index){    
    ui->holeCheckBox->setChecked(m_holeWasActivated);
    if ( index == 8 ) {// hole type
        disconnect(ui->holeCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateHole()));
        ui->holeCheckBox->setChecked(true);
        ui->holeCheckBox->setEnabled(false);
        ui->holeSpinBox->setEnabled(true);
        ui->viaCheckBox->setEnabled(false);
        connect(ui->holeCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateHole()));
    }else{
       ui->holeCheckBox->setEnabled(true);
       ui->viaCheckBox->setEnabled(m_holeWasActivated);
       ui->holeCheckBox->setChecked(m_holeWasActivated);
    }

    if ( m_group->isToolItem() ){
        m_group->setType((PadItem::PadType)index);
        for ( int i = 0; i < m_bus.count(); i++ )
            m_bus.at(i)->setType((PadItem::PadType)index);
    }else{
        EditPadUndoCommand *cmd = new EditPadUndoCommand(m_group,
                                                         ui->nameLineEdit->text(),
                                                         (PadItem::PadType)ui->typeComboBox->currentIndex(),
                                                         ui->widthSpinBox->rawValue(),
                                                         ui->holeCheckBox->isChecked(),
                                                         ui->holeSpinBox->rawValue(),
                                                         ui->viaCheckBox->isChecked());
        qApp->currentTypon()->undoStack()->push(cmd);
    }
}

void PadTool::handleMousePressEvent(QMouseEvent *event){
    if ( isEnabled() ){
        if ( event->button() == Qt::LeftButton || event->button() == Qt::MidButton )
            MYToolBase::mousePressEventTool(event);
        if ( event->button() == Qt::LeftButton ){
            if ( ! qApp->currentTypon()->typonScene()->sceneRect().contains(qApp->currentTypon()->typonView()->mapToScene(event->pos())) )
                return;
            QPointF pos = qApp->currentTypon()->typonView()->nearestGridPoint(
                              qApp->currentTypon()->typonView()->mapToScene(event->pos()));
            if ( ui->viaCheckBox->isChecked() ){
                AddPadUndoCommand *padCommand = new AddPadUndoCommand(m_group,pos, qApp->currentTypon()->typonScene()->layers()->viaLayer());
                qApp->currentTypon()->undoStack()->push(padCommand);
            }else{
                AddPadUndoCommand *padCommand = new AddPadUndoCommand(m_group,pos, qApp->currentTypon()->typonScene()->layers()->activeLayer());
                qApp->currentTypon()->undoStack()->push(padCommand);
            }
            for ( int i = 0; i < m_bus.count(); i++ ){
                QPointF busPoint  = qApp->currentTypon()->typonView()->nearestGridPoint(
                            qApp->currentTypon()->typonView()->mapToScene(event->pos()));
                if ( ui->busComboBox->currentIndex() == 0 ) // Horizontal bus
                    busPoint = busPoint + QPointF((i+1)*ui->busSpacingSpinBox->rawValue(),0);
                else
                    busPoint = busPoint + QPointF(0,(i+1)*ui->busSpacingSpinBox->rawValue());
                if ( ui->viaCheckBox->isChecked() ){
                    AddPadUndoCommand *padCommand = new AddPadUndoCommand(m_bus.at(i),busPoint, qApp->currentTypon()->typonScene()->layers()->viaLayer());
                    qApp->currentTypon()->undoStack()->push(padCommand);
                }else{
                    AddPadUndoCommand *padCommand = new AddPadUndoCommand(m_bus.at(i),busPoint, qApp->currentTypon()->typonScene()->layers()->activeLayer());
                    qApp->currentTypon()->undoStack()->push(padCommand);
                }
            }
        }
    }
}

void PadTool::handleMouseReleaseEvent(QMouseEvent *event){
    if ( event->button() == Qt::LeftButton || event->button() == Qt::MidButton )
        MYToolBase::mouseReleaseEventTool(event);
}

void PadTool::handleMouseMoveEvent(QMouseEvent *event){
    if ( isEnabled() ){
        m_group->setGroupPos(qApp->currentTypon()->typonView()->nearestGridPoint(
                          qApp->currentTypon()->typonView()->mapToScene(event->pos())));
        for ( int i = 0; i < m_bus.count(); i++ ){
            QPointF busPoint  = qApp->currentTypon()->typonView()->nearestGridPoint(
                        qApp->currentTypon()->typonView()->mapToScene(event->pos()));
            if ( ui->busComboBox->currentIndex() == 0 ) // Horizontal bus
                busPoint = busPoint + QPointF((i+1)*ui->busSpacingSpinBox->rawValue(),0);
            else
                busPoint = busPoint + QPointF(0,(i+1)*ui->busSpacingSpinBox->rawValue());
            m_bus.at(i)->setGroupPos(busPoint);
        }
    }
    MYToolBase::mouseMoveEventTool(event);
}

void PadTool::handleWheelEvent(QWheelEvent *event){
    if ( isEnabled() ){
        m_group->setPos(qApp->currentTypon()->typonView()->nearestGridPoint(
                          qApp->currentTypon()->typonView()->mapToScene(event->pos())));
        for ( int i = 0; i < m_bus.count(); i++ ){
            QPointF busPoint  = qApp->currentTypon()->typonView()->nearestGridPoint(
                        qApp->currentTypon()->typonView()->mapToScene(event->pos()));
            if ( ui->busComboBox->currentIndex() == 0 ) // Horizontal bus
                busPoint = busPoint + QPointF((i+1)*ui->busSpacingSpinBox->rawValue(),0);
            else
                busPoint = busPoint + QPointF(0,(i+1)*ui->busSpacingSpinBox->rawValue());
            m_bus.at(i)->setPos(busPoint);
        }
    }
    MYToolBase::mouseWheelEventTool(event);
}

void PadTool::handleKeyPressEvent(QKeyEvent *event){
    if ( isEnabled() ){
        if (event->key() == Qt::Key_W ) {
            if ( event->modifiers().testFlag(Qt::ShiftModifier) )
                ui->widthSpinBox->stepUp();
            else
                ui->widthSpinBox->stepDown();
        }
        if (event->key() == Qt::Key_Space ) {
            if ( event->modifiers().testFlag(Qt::ShiftModifier) )
                swicthToNextType();
            else
                swicthToPreviousType();
        }
        // rotate by 45° on r key
        if (event->key() == Qt::Key_R ) {
            if ( event->modifiers().testFlag(Qt::ShiftModifier) )
                m_group->rotate(45,m_group->groupBoundingRect().center());
            else
                m_group->rotate(-45,m_group->groupBoundingRect().center());
            for ( int i = 0; i < m_bus.count(); i++ ){
                if ( event->modifiers().testFlag(Qt::ShiftModifier) )
                    m_bus.at(i)->rotate(45,m_bus.at(i)->groupBoundingRect().center());
                else
                    m_bus.at(i)->rotate(-45,m_bus.at(i)->groupBoundingRect().center());
            }
        }
    }
    MYToolBase::keyPressEventTool(event);
}

void PadTool::handleKeyReleaseEvent(QKeyEvent *event){
    MYToolBase::keyReleaseEventTool(event);
}

void PadTool::handleEnterEvent(QEvent *event){
    if ( isEnabled() ){
        if ( ui->viaCheckBox->isChecked() )
            m_group->setLayer(qApp->currentTypon()->typonScene()->layers()->viaLayer());
        else
            m_group->setLayer(qApp->currentTypon()->typonScene()->layers()->activeLayer());
        for ( int i = 0; i < m_bus.count(); i++ ){
            if ( ui->viaCheckBox->isChecked() )
                m_bus.at(i)->setLayer(qApp->currentTypon()->typonScene()->layers()->viaLayer());
            else
                m_bus.at(i)->setLayer(qApp->currentTypon()->typonScene()->layers()->activeLayer());
        }
    }
    MYToolBase::enterEventTool(event);
}

void PadTool::handleLeaveEvent(QEvent *event){
    if ( isEnabled() ){
        m_group->setLayer(0);
        for ( int i = 0; i < m_bus.count(); i++ )
            m_bus.at(i)->setLayer(0);
    }
    MYToolBase::leaveEventTool(event);
}

void PadTool::swicthToNextType(){
    int i = ui->typeComboBox->currentIndex();
    if ( i == 0 )
        i = ui->typeComboBox->count();
    i--;
    ui->typeComboBox->setCurrentIndex(i);
}

void PadTool::swicthToPreviousType(){
    int i = ui->typeComboBox->currentIndex();
    if ( i == ui->typeComboBox->count() -1 )
        i = -1;
    i++;
    ui->typeComboBox->setCurrentIndex(i);
}

void PadTool::updateBus(){  
    if ( ui->busCheckBox->isChecked() ){
        ui->busComboBox->setEnabled(true);
        ui->busSpinBox->setEnabled(true);
        ui->busSpacingSpinBox->setEnabled(true);
    }else{
        ui->busComboBox->setEnabled(false);
        ui->busSpinBox->setEnabled(false);
        ui->busSpacingSpinBox->setEnabled(false);
    }

    clearBus();
    if ( ui->busCheckBox->isChecked() ){
        for (int i = 1; i < ui->busSpinBox->value(); i++){
            PadGroup *p = new PadGroup();
            p->setToolItem(true);
            m_bus.append(p);
        }
        for (int i = 0; i < m_bus.count(); i++){
            m_bus.at(i)->setTransform(m_group->pad()->transform());
            m_bus.at(i)->rotate(m_group->pad()->rotation(), m_bus.at(i)->groupBoundingRect().center());
        }
        updatePadType(ui->typeComboBox->currentIndex());
        updateWidth();
        updateHole();
        updateVia();
        updateHoleWidth();
    }   
}

void PadTool::clearBus(){
    for (int i =0; i < m_bus.count(); i++ ){
        m_bus.at(i)->removeFromGroup(m_bus.at(i)->pad());
        m_bus.at(i)->removeFromGroup(m_bus.at(i)->hole());
        if ( ! m_bus.at(i)->pad()->parentItem() )
            delete m_bus.at(i)->pad();
        if ( ! m_bus.at(i)->hole()->parentItem() )
            delete m_bus.at(i)->hole();

        delete m_bus.at(i);
    }
    m_bus.clear();
}

void PadTool::onNameChanged(){
    EditPadUndoCommand *cmd = new EditPadUndoCommand(m_group,
                                                     ui->nameLineEdit->text(),
                                                     (PadItem::PadType)ui->typeComboBox->currentIndex(),
                                                     ui->widthSpinBox->rawValue(),
                                                     ui->holeCheckBox->isChecked(),
                                                     ui->holeSpinBox->rawValue(),
                                                     ui->viaCheckBox->isChecked());
    qApp->currentTypon()->undoStack()->push(cmd);
    emit nameChanged(m_group);
}
