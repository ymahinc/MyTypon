#include "tracktool.h"
#include "ui_tracktool.h"
#include "global.h"
#include "utils/trackcurrentcalculator.h"
#include "utils/mytyponunits.h"
#include "addtrackundocommand.h"
#include "edittrackundocommand.h"
#include "items/trackitem/angle.h"
//#include "addbusundocommand.h"
#include <QDebug>

TrackTool::TrackTool(QWidget *parent, Track *item)
    : MYToolBase(parent), ui(new Ui::TrackTool) {
    ui->setupUi(this);    
    m_isCreatingTrack = false;
    setAcceptRubberband(false);
    setAcceptAutoExpand(true,MYToolBase::Auto);
    setToolName(tr("Track"));
    setToolIcon(QIcon(":/pictures/draw_line.png"));
    m_cursor = Qt::CrossCursor;

    // fill the table with widgets (hidden in page2 of stacked widget)
    ui->tableWidget->setCellWidget(0,1,ui->nameLineEdit);
    ui->tableWidget->setCellWidget(1,1,ui->typeComboBox);
    ui->tableWidget->setCellWidget(2,1,ui->widthSpinBox);
    ui->tableWidget->setCellWidget(3,1,ui->busCheckBox);
    ui->tableWidget->setCellWidget(4,1,ui->busTypeComboBox);
    ui->tableWidget->setCellWidget(5,1,ui->busSpinBox);
    ui->tableWidget->setCellWidget(6,1,ui->spacingSpinBox);

    m_mode = Simple;
    m_trackType = Track::Flat;

    if ( item == 0 ){
        // as it is a tool item hide name widget, we dont want to edit this now
        ui->tableWidget->setRowHidden(0,true);
        ui->widthSpinBox->setRawValue(25);
        ui->spacingSpinBox->setRawValue(qApp->workSpace()->grid->xStepGrid());
        // m_track is the track that will be added to the typon
        m_track = new Track(0);
        m_track->setToolItem(true);
        m_track->setWidth(ui->widthSpinBox->rawValue());
        // tempTrack is the track segment "under the mouse" wich will be merged with track
        // track is only added to the typon when edit ending
        m_tempTrack = new Track(0);
        m_tempTrack->setToolItem(true);
        m_tempTrack->setWidth(ui->widthSpinBox->rawValue());
        // temp track is made of 3 angles
        m_firstAngle = new Angle(m_tempTrack);
        m_midAngle = new Angle(m_tempTrack);
        m_lastAngle = new Angle(m_tempTrack);
        m_tempTrack->appendAngle(m_firstAngle,false);
        m_tempTrack->appendAngle(m_midAngle,false);
        m_tempTrack->appendAngle(m_lastAngle,false);
    }else{
        ui->tableWidget->setRowHidden(3,true);
        ui->tableWidget->setRowHidden(4,true);
        ui->tableWidget->setRowHidden(5,true);
        ui->tableWidget->setRowHidden(6,true);
        ui->typeComboBox->removeItem(1);
        ui->typeComboBox->removeItem(1);
        ui->typeComboBox->removeItem(1);
        ui->typeComboBox->removeItem(1);
        m_track = item;
        updateTool();
    }

    m_track->setTool(this);

    connect(ui->widthSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateCurrentMax()));
    connect(ui->widthSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateWidth()));
    connect(ui->typeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateTrackType(int)));
    connect(ui->busSpinBox,SIGNAL(valueChanged(int)),this,SLOT(updateBus()));
    connect(ui->busCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateBusWidgets()));
}

void TrackTool::updateBusWidgets(){
    ui->busSpinBox->setEnabled(ui->busCheckBox->isChecked());
    ui->spacingSpinBox->setEnabled(ui->busCheckBox->isChecked());
    ui->busTypeComboBox->setEnabled(ui->busCheckBox->isChecked());
    if ( ui->busCheckBox->isChecked() )
        updateBus();
    else
        clearBusVectors();
}

void TrackTool::updateTool(){
    disconnect(ui->widthSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateCurrentMax()));
    disconnect(ui->widthSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateWidth()));
    disconnect(ui->typeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateTrackType(int)));
    disconnect(ui->busSpinBox,SIGNAL(valueChanged(int)),this,SLOT(updateBus()));
    disconnect(ui->busCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateBusWidgets()));
    disconnect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged()));

    ui->nameLineEdit->setText(m_track->itemName());
    ui->typeComboBox->setCurrentIndex((int)m_track->trackType());
    if ( ui->widthSpinBox->rawValue() != m_track->width() )
        ui->widthSpinBox->setRawValue(m_track->width());

    emit nameChanged(m_track);

    updateCurrentMax();

    connect(ui->widthSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateCurrentMax()));
    connect(ui->widthSpinBox,SIGNAL(rawValueChanged()),this,SLOT(updateWidth()));
    connect(ui->typeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateTrackType(int)));
    connect(ui->busSpinBox,SIGNAL(valueChanged(int)),this,SLOT(updateBus()));
    connect(ui->busCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateBusWidgets()));
    connect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged()));
}

TrackTool::~TrackTool(){
    delete ui;
    m_track->setTool(0);
    clearBusVectors();
    if ( m_track->isToolItem() && ! m_track->parentItem() )
        delete m_track;
}

void TrackTool::setMode(Mode mode){
    ui->typeComboBox->setCurrentIndex((int) mode);
}

// update mid and last point when mouse is moving and a track is being edited
void TrackTool::updateSecondPoint(Track *track, QPoint pos, Angle *firstAngle, Angle *midAngle, Angle *lastAngle, int index){
    QPointF posScene;
    // if bus vector, add a step for each new track: track at 0 in vector, add one step, track at 1 add 2 steps, etc....
    index++;

    // compute new position
    posScene = qApp->currentTypon()->typonView()->nearestGridPoint(
                    qApp->currentTypon()->typonView()->mapToScene(pos));

    // update Y value with spacing step value for bus management if bus is horizontal
    if ( ui->busTypeComboBox->currentIndex() == 0 )
        posScene.setY(posScene.y()+index*ui->spacingSpinBox->rawValue());

    // update X value with ystep value for bus management if bus is vertical
    if ( ui->busTypeComboBox->currentIndex() == 1 )
        posScene.setX(posScene.x()+index*ui->spacingSpinBox->rawValue());

    // if mode is smiple (no intermediate angle, mid angle is equal to first angle, only update last angle
    if ( m_mode == Simple || m_mode == Curved || m_mode == Curved2 ){
        track->updateAnglePos(midAngle,firstAngle->scenePos());
        track->updateAnglePos(lastAngle,posScene);
    }

    if ( m_mode == RightAngle1 ){
        QPointF midPoint = QPointF(posScene.x(),firstAngle->scenePos().y());
        track->updateAnglePos(midAngle,midPoint);
        track->updateAnglePos(lastAngle,posScene);
    }

    if ( m_mode == RightAngle2 ){
        QPointF midPoint = QPointF(firstAngle->scenePos().x(),posScene.y());
        track->updateAnglePos(midAngle,midPoint);
        track->updateAnglePos(lastAngle,posScene);
    }

    if ( m_mode == OneAngle1 ){
        int deltaX = 0;
        int deltaY = qAbs(posScene.y() - firstAngle->scenePos().y());
        QPointF midPoint = firstAngle->scenePos();
        if ( (posScene.x() < firstAngle->scenePos().x()) && (posScene.y() < firstAngle->scenePos().y()) ){
            int newX = posScene.x() + deltaY;
            if ( newX > firstAngle->scenePos().x() ){
                deltaX = newX - firstAngle->scenePos().x();
                midPoint.setX(firstAngle->scenePos().x());
                midPoint.setY(firstAngle->scenePos().y()-deltaX);
            }else{
                midPoint.setX(posScene.x() + deltaY);
                midPoint.setY(firstAngle->scenePos().y());
            }
        }
        if ( (posScene.x() < firstAngle->scenePos().x()) && (posScene.y() > firstAngle->scenePos().y()) ){
            int newX = posScene.x() + deltaY;
            if ( newX > firstAngle->scenePos().x() ){
                deltaX = newX - firstAngle->scenePos().x();
                midPoint.setX(firstAngle->scenePos().x());
                midPoint.setY(firstAngle->scenePos().y()+deltaX);
            }else{
                midPoint.setX(posScene.x() + deltaY);
                midPoint.setY(firstAngle->scenePos().y());
            }
        }
        if ( (posScene.x() > firstAngle->scenePos().x()) && (posScene.y() < firstAngle->scenePos().y()) ){
            int newX = posScene.x() - deltaY;
            if ( newX < firstAngle->scenePos().x() ){
                deltaX = firstAngle->scenePos().x() - newX;
                midPoint.setX(firstAngle->scenePos().x());
                midPoint.setY(firstAngle->scenePos().y()-deltaX);
            }else{
                midPoint.setX(posScene.x() - deltaY);
                midPoint.setY(firstAngle->scenePos().y());
            }
        }
        if ( (posScene.x() > firstAngle->scenePos().x()) && (posScene.y() > firstAngle->scenePos().y()) ){
            int newX = posScene.x() - deltaY;
            if ( newX < firstAngle->scenePos().x() ){
                deltaX = firstAngle->scenePos().x() - newX;
                midPoint.setX(firstAngle->scenePos().x());
                midPoint.setY(firstAngle->scenePos().y()+deltaX);
            }else{
                midPoint.setX(posScene.x() - deltaY);
                midPoint.setY(firstAngle->scenePos().y());
            }
        }
        track->updateAnglePos(midAngle,midPoint);
        track->updateAnglePos(lastAngle,posScene);
    }

    if ( m_mode == OneAngle2 ){
        int deltaX = 0;
        int deltaY = qAbs(posScene.y() - firstAngle->scenePos().y());
        QPointF midPoint = firstAngle->scenePos();
        if ( (posScene.x() < firstAngle->scenePos().x()) && (posScene.y() < firstAngle->scenePos().y()) ){
            int newX = firstAngle->scenePos().x() - deltaY;
            if ( newX < posScene.x() ){
                deltaX = posScene.x() - newX;
                midPoint.setX(posScene.x());
                midPoint.setY(posScene.y()+deltaX);
            }else{
                midPoint.setX(firstAngle->scenePos().x() - deltaY);
                midPoint.setY(posScene.y());
            }
        }
        if ( (posScene.x() < firstAngle->scenePos().x()) && (posScene.y() > firstAngle->scenePos().y()) ){
            int newX = firstAngle->scenePos().x() - deltaY;
            if ( newX < posScene.x() ){
                deltaX = posScene.x() - newX;
                midPoint.setX(posScene.x());
                midPoint.setY(posScene.y()-deltaX);
            }else{
                midPoint.setX(firstAngle->scenePos().x() - deltaY);
                midPoint.setY(posScene.y());
            }
        }
        if ( (posScene.x() > firstAngle->scenePos().x()) && (posScene.y() < firstAngle->scenePos().y()) ){
            int newX = firstAngle->scenePos().x() + deltaY;
            if ( newX > posScene.x() ){
                deltaX = newX - posScene.x();
                midPoint.setX(posScene.x());
                midPoint.setY(posScene.y()+deltaX);
            }else{
                midPoint.setX(firstAngle->scenePos().x() + deltaY);
                midPoint.setY(posScene.y());
            }
        }
        if ( (posScene.x() > firstAngle->scenePos().x()) && (posScene.y() >firstAngle->scenePos().y()) ){
            int newX = firstAngle->scenePos().x() + deltaY;
            if ( newX > posScene.x() ){
                deltaX = newX - posScene.x();
                midPoint.setX(posScene.x());
                midPoint.setY(posScene.y()-deltaX);
            }else{
                midPoint.setX(firstAngle->scenePos().x() + deltaY);
                midPoint.setY(posScene.y());
            }
        }
        track->updateAnglePos(midAngle,midPoint);
        track->updateAnglePos(lastAngle,posScene);
    }
}

// update track type
void TrackTool::updateTrackType(int index){    
    // if tool item we are adding new track
    if ( m_track->isToolItem() ){
        // depending of track type, set the track and temp track in flat or curved mode accordingly
        // do it also for all bus tracks
        switch (index) {
        case 0:{
            m_mode = Simple;
            m_track->setType(Track::Flat);
            m_tempTrack->setType(Track::Flat);
            for(int i = 0; i < m_busTrackVector.count(); i++){
                m_busTrackVector.at(i)->setType(Track::Flat);
                m_busTempTrackVector.at(i)->setType(Track::Flat);
            }
            break;}
        case 1:{
            m_mode = OneAngle1;
            m_track->setType(Track::Flat);
            m_tempTrack->setType(Track::Flat);
            for(int i = 0; i < m_busTrackVector.count(); i++){
                m_busTrackVector.at(i)->setType(Track::Flat);
                m_busTempTrackVector.at(i)->setType(Track::Flat);
            }
            break;}
        case 2:{
            m_mode = OneAngle2;
            m_track->setType(Track::Flat);
            m_tempTrack->setType(Track::Flat);
            for(int i = 0; i < m_busTrackVector.count(); i++){
                m_busTrackVector.at(i)->setType(Track::Flat);
                m_busTempTrackVector.at(i)->setType(Track::Flat);
            }
            break;}
        case 3:{
            m_track->setType(Track::Flat);
            m_tempTrack->setType(Track::Flat);
            for(int i = 0; i < m_busTrackVector.count(); i++){
                m_busTrackVector.at(i)->setType(Track::Flat);
                m_busTempTrackVector.at(i)->setType(Track::Flat);
            }
            m_mode = RightAngle1;
            break;}
        case 4:{
            m_track->setType(Track::Flat);
            m_tempTrack->setType(Track::Flat);
            for(int i = 0; i < m_busTrackVector.count(); i++){
                m_busTrackVector.at(i)->setType(Track::Flat);
                m_busTempTrackVector.at(i)->setType(Track::Flat);
            }
            m_mode = RightAngle2;
            break;}
        case 5:{
            m_mode = Curved;
            m_track->setType(Track::Curved);
            m_tempTrack->setType(Track::Curved);
            for(int i = 0; i < m_busTrackVector.count(); i++){
                m_busTrackVector.at(i)->setType(Track::Curved);
                m_busTempTrackVector.at(i)->setType(Track::Curved);
            }
            break;}
        case 6:{
            m_mode = Curved2;
            m_track->setType(Track::Curved2);
            m_tempTrack->setType(Track::Curved2);
            for(int i = 0; i < m_busTrackVector.count(); i++){
                m_busTrackVector.at(i)->setType(Track::Curved2);
                m_busTempTrackVector.at(i)->setType(Track::Curved2);
            }
            break;}
        }
        // update secondpoint, it will recalculate mid angle position if mode has changed
        updateSecondPoint(m_tempTrack,qApp->currentTypon()->typonView()->lastMousePos(),
                          m_firstAngle,m_midAngle,m_lastAngle);
        for(int i = 0; i < m_busTrackVector.count(); i++){
            updateSecondPoint(m_busTempTrackVector.at(i),qApp->currentTypon()->typonView()->lastMousePos(),
                              m_busFirstAngleVector.at(i),m_busMidAngleVector.at(i),m_busLastAngleVector.at(i));
        }
    }else{
        // we are editing existing track, add an edit undo command
        m_trackType = Track::Flat;
        if ( index == 1 )
            m_trackType = Track::Curved;
        if ( index == 2 )
            m_trackType = Track::Curved2;
        EditTrackUndoCommand *trackEditCommand = new EditTrackUndoCommand(m_track,
                                                                       ui->nameLineEdit->text(),
                                                                       ui->widthSpinBox->rawValue(),
                                                                       m_trackType);
        qApp->undoGroup()->activeStack()->push(trackEditCommand);
    }
}

// calculate the max current admissible for the current track and update widget's label
void TrackTool::updateCurrentMax(){
    if ( qApp->currentTypon() == 0 )
        return;

    Layer *lay = 0;
    if ( m_track->isToolItem() )
        lay = qApp->currentTypon()->typonScene()->layers()->activeLayer();
    else
        lay = m_track->layer();

    QString layerTypeStr;
    if ( lay->isSystem() )
        layerTypeStr = tr("External");
    else
        layerTypeStr = tr("Internal");
    double Imax = TrackCurrentCalculator::currentFromTrackWidth(
                ui->widthSpinBox->rawValue(),
                MYTyponUnits::unitToScene(MYTyponUnits::MM,qApp->currentTypon()->copperThickness()*0.001),
                qApp->workSpace()->tempRise, !lay->isSystem());
    ui->currentLabel->setText(tr("Imax: %1A for %2°C rise.\nThickness: %3µm\n%4 layer").arg(QString::number(Imax,'f',2))
                              .arg(qApp->workSpace()->tempRise)
                              .arg(QString::number(qApp->currentTypon()->copperThickness()))
                              .arg(layerTypeStr));
}

// update the track width
void TrackTool::updateWidth(){
    // if creating track, update track width and all bus tracks width also
    if ( m_track->isToolItem() ){
        m_track->setWidth(ui->widthSpinBox->rawValue());
        m_tempTrack->setWidth(ui->widthSpinBox->rawValue());
        for(int i = 0; i < m_busTrackVector.count(); i++){
            m_busTrackVector.at(i)->setWidth(ui->widthSpinBox->rawValue());
            m_busTempTrackVector.at(i)->setWidth(ui->widthSpinBox->rawValue());
        }
    }else{
        // we are editing width of an existing track
        EditTrackUndoCommand *trackEditCommand = new EditTrackUndoCommand(m_track,
                                                                       ui->nameLineEdit->text(),
                                                                       ui->widthSpinBox->rawValue(),
                                                                       m_trackType);
        qApp->undoGroup()->activeStack()->push(trackEditCommand);
    }
}

void TrackTool::handleMousePressEvent(QMouseEvent *event){
    if ( event->button() == Qt::LeftButton ){
        if ( ! isEnabled() )
            return;
        if ( ! qApp->currentTypon()->typonScene()->sceneRect().contains(qApp->currentTypon()->typonView()->mapToScene(event->pos())) )
            return;
        // if we are not currently adding a track, add a new one
        if ( ! m_isCreatingTrack ){
            updateBus(); // clear bus if bus is disabled else create new bus tracks ( as much as specified in spinbox)
            // add tracks and temp tracks to the scene
            m_track->setParentItem(qApp->currentTypon()->typonScene()->layers()->activeLayer());
            m_tempTrack->setParentItem(qApp->currentTypon()->typonScene()->layers()->activeLayer());
            for(int i = 0; i < m_busTrackVector.count(); i++){
                m_busTrackVector.at(i)->setParentItem(qApp->currentTypon()->typonScene()->layers()->activeLayer());
                m_busTempTrackVector.at(i)->setParentItem(qApp->currentTypon()->typonScene()->layers()->activeLayer());
            }

            // intialize track and temp track (all angles at the same pos: click pos)
            m_isCreatingTrack = true;
            QPointF newPoint= qApp->currentTypon()->typonView()->nearestGridPoint(
                        qApp->currentTypon()->typonView()->mapToScene(event->pos()));
            m_tempTrack->updateAnglePos(m_firstAngle,newPoint);
            m_tempTrack->updateAnglePos(m_midAngle,newPoint);
            m_tempTrack->updateAnglePos(m_lastAngle,newPoint);
            m_track->appendAngle(newPoint);

            // intialize all bus tracks and temp tracks
            for(int i = 0; i < m_busTrackVector.count(); i++){
                QPointF busPoint = newPoint;
                // add spacing beetween bus tracks accordingly to spacing value
                if ( ui->busTypeComboBox->currentIndex() == 1 )
                    busPoint.setX(newPoint.x()+(i+1)*ui->spacingSpinBox->rawValue());
                if ( ui->busTypeComboBox->currentIndex() == 0 )
                    busPoint.setY(newPoint.y()+(i+1)*ui->spacingSpinBox->rawValue());
                // update angles for bus tracks (all three angles have the same value on begin editing)
                m_busTempTrackVector.at(i)->updateAnglePos(m_busFirstAngleVector.at(i),busPoint);
                m_busTempTrackVector.at(i)->updateAnglePos(m_busMidAngleVector.at(i),busPoint);
                m_busTempTrackVector.at(i)->updateAnglePos(m_busLastAngleVector.at(i),busPoint);
                m_busTrackVector.at(i)->appendAngle(busPoint);
            }
        }else{
            // we are already creating a track, append temp track to the "container" track
            updateSecondPoint(m_tempTrack,event->pos(),
                              m_firstAngle,m_midAngle,m_lastAngle);
            m_track->append(m_tempTrack);
            // intialize all angles of temp track at the last click pos
            m_tempTrack->updateAnglePos(m_firstAngle,m_lastAngle->scenePos());
            m_tempTrack->updateAnglePos(m_midAngle,m_lastAngle->scenePos());

            // do it for all bus tracks
            for(int i = 0; i < m_busTrackVector.count(); i++){
                updateSecondPoint(m_busTempTrackVector.at(i),event->pos(),
                                  m_busFirstAngleVector.at(i),m_busMidAngleVector.at(i),m_busLastAngleVector.at(i),i);
                m_busTrackVector.at(i)->append(m_busTempTrackVector.at(i));
                m_busTempTrackVector.at(i)->updateAnglePos(m_busFirstAngleVector.at(i),m_busLastAngleVector.at(i)->scenePos());
                m_busTempTrackVector.at(i)->updateAnglePos(m_busMidAngleVector.at(i),m_busLastAngleVector.at(i)->scenePos());
            }
        }
    }

    if ( event->button() == Qt::RightButton ){
        addTrackToTypon();
    }
    MYToolBase::mousePressEventTool(event);
}

void TrackTool::handleMouseReleaseEvent(QMouseEvent *event){
    if ( event->button() == Qt::LeftButton || event->button() == Qt::MidButton )
        MYToolBase::mouseReleaseEventTool(event);
}

void TrackTool::handleMouseMoveEvent(QMouseEvent *event){
    // if we are creating a track, update mid angle and last angle pos, idem for bus
    if ( m_isCreatingTrack ){
        updateSecondPoint(m_tempTrack,event->pos(),
                          m_firstAngle,m_midAngle,m_lastAngle);
        for(int i = 0; i < m_busTrackVector.count(); i++){
            updateSecondPoint(m_busTempTrackVector.at(i),event->pos(),
                              m_busFirstAngleVector.at(i),m_busMidAngleVector.at(i),m_busLastAngleVector.at(i),i);
        }
    }
    MYToolBase::mouseMoveEventTool(event);
}

void TrackTool::handleWheelEvent(QWheelEvent *event){
    // if we are creating a track, update mid angle and last angle pos, idem for bus
    if ( m_isCreatingTrack ){
        updateSecondPoint(m_tempTrack,event->pos(),
                          m_firstAngle,m_midAngle,m_lastAngle);
        for(int i = 0; i < m_busTrackVector.count(); i++){
            updateSecondPoint(m_busTempTrackVector.at(i),event->pos(),
                              m_busFirstAngleVector.at(i),m_busMidAngleVector.at(i),m_busLastAngleVector.at(i),i);
        }
    }
    MYToolBase::mouseWheelEventTool(event);
}

void TrackTool::addTrackToTypon(){
    // if we are creating track, add a new track to the scene
    if ( m_isCreatingTrack ){
        // track is only added if it has at least two angles
        if ( m_track->angles().count() > 1 ){
            // if no bus, only add the track
            if ( !ui->busCheckBox->isChecked() ) {
                AddTrackUndoCommand *addTrackCommand = new AddTrackUndoCommand(m_track);
                qApp->undoGroup()->activeStack()->push(addTrackCommand);
            }else{ // we are creating a bus
                // start an undo macro
                qApp->undoGroup()->activeStack()->beginMacro(tr("Add Bus"));
                // add current track
                AddTrackUndoCommand *addTrackCommand = new AddTrackUndoCommand(m_track);
                qApp->undoGroup()->activeStack()->push(addTrackCommand);
                // then add all bus tracks
                for(int i = 0; i < m_busTrackVector.count(); i++){
                    AddTrackUndoCommand *addTrackCommand = new AddTrackUndoCommand(m_busTrackVector.at(i));
                    qApp->undoGroup()->activeStack()->push(addTrackCommand);
                }
                // end macro
                qApp->undoGroup()->activeStack()->endMacro();
            }
        }

        // clear all working tracks and bus tracks
        m_track->clearTrack();

        m_isCreatingTrack = false;
        qApp->currentTypon()->typonScene()->removeItem(m_track);
        qApp->currentTypon()->typonScene()->removeItem(m_tempTrack);

        for(int i = 0; i < m_busTrackVector.count(); i++){
            m_busTrackVector.at(i)->clearTrack();
            qApp->currentTypon()->typonScene()->removeItem( m_busTrackVector.at(i));
            qApp->currentTypon()->typonScene()->removeItem( m_busTempTrackVector.at(i));
        }
        clearBusVectors();
    }
}

void TrackTool::handleKeyPressEvent(QKeyEvent *event){
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
        if (event->key() == Qt::Key_Escape ) {
            addTrackToTypon();
        }
    }
    MYToolBase::keyPressEventTool(event);
}

void TrackTool::handleKeyReleaseEvent(QKeyEvent *event){
    MYToolBase::keyReleaseEventTool(event);
}

void TrackTool::swicthToNextType(){
    int i = ui->typeComboBox->currentIndex();
    if ( i == 0 )
        i = ui->typeComboBox->count();
    i--;
    ui->typeComboBox->setCurrentIndex(i);
}

void TrackTool::swicthToPreviousType(){
    int i = ui->typeComboBox->currentIndex();
    if ( i == ui->typeComboBox->count() -1 )
        i = -1;
    i++;
    ui->typeComboBox->setCurrentIndex(i);
}

// remove all tracks from bus
// clear all vectors
void TrackTool::clearBusVectors(){
    for (int i = 0; i < m_busTrackVector.count(); i++){
        if ( ! m_busTrackVector.at(i)->parentItem() )
            delete m_busTrackVector.at(i);
        if ( ! m_busTempTrackVector.at(i)->parentItem() )
            delete m_busTempTrackVector.at(i);
    }
    m_busTrackVector.clear();
    m_busTempTrackVector.clear();
    m_busFirstAngleVector.clear();
    m_busMidAngleVector.clear();
    m_busLastAngleVector.clear();
}

// create bus if necessary
void TrackTool::updateBus(){
    // clear existing bus
    clearBusVectors();
    // if bus is not activated, return
    if ( ! ui->busCheckBox->isChecked() )
        return;
    // create as many tracks as needed
    for (int i = 0; i < ui->busSpinBox->value()-1; i++){
        // track is the working "container track"
        Track *track = new Track(0);
        track->setWidth(ui->widthSpinBox->rawValue());
        track->setToolItem(true);
        // add it to the corresponding vector
        m_busTrackVector.append(track);

        // temp track is the track "under the mouse"
        Track *tempTrack = new Track(0);
        tempTrack->setWidth(ui->widthSpinBox->rawValue());
        tempTrack->setToolItem(true);
        // add it to the corresponding vector
        m_busTempTrackVector.append(tempTrack);

        // each temp track has three angles, each added in its corresponding vectoe
        Angle *firstAngle = new Angle(tempTrack);
        Angle *midAngle = new Angle(tempTrack);
        Angle *lastAngle = new Angle(tempTrack);
        m_busFirstAngleVector.append(firstAngle);
        m_busMidAngleVector.append(midAngle);
        m_busLastAngleVector.append(lastAngle);
        tempTrack->appendAngle(firstAngle,false);
        tempTrack->appendAngle(midAngle,false);
        tempTrack->appendAngle(lastAngle,false);

        // update type
        updateTrackType(ui->typeComboBox->currentIndex());
    }
}

void TrackTool::handleEnterEvent(QEvent *event){
    MYToolBase::enterEventTool(event);
}

void TrackTool::handleLeaveEvent(QEvent *event){
    MYToolBase::leaveEventTool(event);
}

// change name value, can only occured on an existing item on the scene
void TrackTool::onNameChanged(){
    EditTrackUndoCommand *trackEditCommand = new EditTrackUndoCommand(m_track,
                                                                   ui->nameLineEdit->text(),
                                                                   ui->widthSpinBox->rawValue(),
                                                                   m_trackType);
    qApp->undoGroup()->activeStack()->push(trackEditCommand);
    emit nameChanged(m_track);
}
