#include "mytypon.h"
#include "ui_mytypon.h"
#include "global.h"
#include "items/textitem/textitem.h"
#include "items/paditem/padgroup.h"
#include "items/trackitem/track.h"
#include "items/drawitem.h"
#include "tools/TextTool/addtextundocommand.h"
#include "tools/PadTool/addpadundocommand.h"
#include "tools/TrackTool/addtrackundocommand.h"
#include "tools/DrawTool/adddrawitemundocommand.h"
#include "tools/GroupTool/groupundocommand.h"

#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <QScrollBar>
#include <QTimer>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMimeData>
#include <QClipboard>

MYTypon::MYTypon(QWidget *parent, QString path) :
    QWidget(parent), ui(new Ui::MYTypon){

    m_path = path;

    instancesCounter++;
    setTyponName(tr("New typon %1").arg(QString::number(instancesCounter)));

    // on the first display, launch a one shot timer so the view is actually centered on origin
    m_firstShow = true;

    ui->setupUi(this);

    // create the undo stack for this typon and add it to the general undogroup
    m_undoStack = new QUndoStack(this);
    qApp->undoGroup()->addStack(m_undoStack);

    // create the load undo stack for this typon. Used when loading a file
    // all items are created and deleted via undo commands, we need this to load items from files without modifying "working" undo stack
    m_loadUndoStack = new QUndoStack(this);

    connect(m_undoStack,SIGNAL(cleanChanged(bool)),this,SLOT(updateTyponName(bool)));

    m_scene = new MYTyponScene(this);
    m_scene->setSceneRect(0,0,50000,50000);

    m_view = new MYTyponView(m_scene, this);

    m_unitsLabel = new MYUnitsMenuLabel(this);

    setTyponSize(qApp->workSpace()->defaultTyponSize);
    setCopperThickness(qApp->workSpace()->copperThickness);

    m_hRuler = new MYTyponRuler(m_scene,this, MYTyponRuler::Horizontal);
    m_hRuler->setFixedHeight(23);
    m_vRuler = new MYTyponRuler(m_scene,this, MYTyponRuler::Vertical);
    m_vRuler->setFixedWidth(23);

    m_layout = new QGridLayout(this);
    m_layout->setHorizontalSpacing(0);
    m_layout->setVerticalSpacing(0);
    m_layout->addWidget(m_unitsLabel, 0, 0);
    m_layout->addWidget(m_view, 1, 1);
    m_layout->addWidget(m_hRuler, 0, 1);
    m_layout->addWidget(m_vRuler, 1, 0);
    setLayout(m_layout);

    // connect general signals to update the widgets
    connect(qApp->workSpace(),SIGNAL(workspaceUpdated()),this,SLOT(updateScene()));
    connect(qApp->workSpace(),SIGNAL(workspaceUpdated()),m_hRuler,SLOT(changeVisibility()));
    connect(qApp->workSpace(),SIGNAL(workspaceUpdated()),m_vRuler,SLOT(changeVisibility()));
    connect(qApp->workSpace(),SIGNAL(workspaceUpdated()),m_unitsLabel,SLOT(changeVisibility()));
    connect(qApp->workSpace(),SIGNAL(workspaceUpdated()),this,SLOT(updateLayersTransparency()));
    connect(m_unitsLabel,SIGNAL(unitChanged(MYTyponUnits::Units)),qApp->workSpace(),SLOT(onUnitChanged(MYTyponUnits::Units)));

    //connect events of this typon to update mainwindow
    connect(m_view,SIGNAL(viewportLeaved()),this,SIGNAL(viewportLeaved()));
    // update rulers according to typonview events
    connect(m_view,SIGNAL(mousePosHasChangedScenePos(QPointF)),m_hRuler,SLOT(updatePos(QPointF)));
    connect(m_view,SIGNAL(mousePosHasChangedScenePos(QPointF)),m_vRuler,SLOT(updatePos(QPointF)));
    connect(m_view,SIGNAL(viewportLeaved()),m_hRuler,SLOT(hideLines()));
    connect(m_view,SIGNAL(viewportLeaved()),m_vRuler,SLOT(hideLines()));
    connect(m_view->horizontalScrollBar(),SIGNAL(valueChanged(int)),m_hRuler->horizontalScrollBar(),SLOT(setValue(int)));
    connect(m_view->verticalScrollBar(),SIGNAL(valueChanged(int)),m_vRuler->verticalScrollBar(),SLOT(setValue(int)));

    //set default zoom
    scale(0.25);
}

MYTypon::~MYTypon(){
    delete ui;
}

QString MYTypon::filePath(){
    return m_path;
}

void MYTypon::setFilePath(QString path){
    m_path = path;
}

void MYTypon::copy(){
    QByteArray array;
    QDataStream outData(&array, QIODevice::WriteOnly);
    foreach (MYItemBase *it, *m_scene->selectedTyponItems() )
        it->save(outData);
    outData << -1;

    QMimeData* mimeData = new QMimeData;
    mimeData->setData("application/myTypon", array);
    qDebug() << array.size();
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->clear();
    clipboard->setMimeData(mimeData);

    qDebug() << "copy";
}

void MYTypon::paste(){
    m_view->setUpdatesEnabled(false);

    QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();

    QStringList formats = mimeData->formats();
    foreach (QString format, formats)
    {
        if (format == "application/myTypon")
        {
            QByteArray itemData = mimeData->data("application/myTypon");
            QDataStream inData(&itemData, QIODevice::ReadOnly);

            qDebug() << itemData.size();

            int itemType;
            inData >> itemType;

            while ( itemType >= 0 ){
                restoreItem(inData, itemType,0);
                inData >> itemType;
            }
        }
    }

    qDebug() << "paste";
    m_view->setUpdatesEnabled(true);
}

void MYTypon::restore(QDataStream &stream, QString path){
    m_view->setUpdatesEnabled(false);
    m_path = path;

    QFileInfo fileInfo(path);
    setTyponName(fileInfo.baseName());
    updateTyponName(true);

    int customLayersCount;
    stream >> customLayersCount;
    for(int i = 0; i < customLayersCount; i++){
        QString name;
        QColor color;
        stream >> name >> color;
        Layer *layer = new Layer(0,name,color);
        m_scene->layers()->addLayer(layer);
    }

    int itemType;
    stream >> itemType;

    while ( itemType >= 0 ){
        restoreItem(stream, itemType,0);
        stream >> itemType;
    }

    /* at the end update all ground planes!! */
    m_view->setUpdatesEnabled(true);
}

void MYTypon::restoreItem(QDataStream &stream, int type, MYItemBase *group){
    switch ( type ){
        case 0: { // group item
            GroupUndoCommand *grpCommand = new GroupUndoCommand(stream,group);
            m_loadUndoStack->push(grpCommand);
            break;
        }
        case 1: { // pad item
            AddPadUndoCommand *padCmd = new AddPadUndoCommand(stream,group);
            m_loadUndoStack->push(padCmd);
            break;
        }
        case 2: { // text item
            AddTextUndoCommand *txtCmd = new AddTextUndoCommand(stream,group);
            m_loadUndoStack->push(txtCmd);
            break;
        }
        case 3: { // track item
            AddTrackUndoCommand *trackCmd = new AddTrackUndoCommand(stream,group);
            m_loadUndoStack->push(trackCmd);
            break;
        }
        case 4: { // track item
            AddDrawItemUndoCommand *drawCmd = new AddDrawItemUndoCommand(stream,group);
            m_loadUndoStack->push(drawCmd);
            break;
        }
    }
}

void MYTypon::save(QDataStream &stream){
    stream << m_scene->layers()->customLayers().count();
    foreach (Layer *lay, m_scene->layers()->customLayers())
        lay->save(stream);

    QList<MYItemBase *> items = qApp->currentTypon()->typonScene()->topMostItems();
    foreach (MYItemBase *it, items )
        it->save(stream);

    stream << -1;
}

void MYTypon::updateScene(){
    m_scene->invalidate(m_scene->sceneRect(),QGraphicsScene::AllLayers);
}

// update transparency of all layers
void MYTypon::updateLayersTransparency(){
    for(int i =0; i < typonScene()->layers()->count(); i++)
        typonScene()->layers()->at(i)->updateTransparency();
    typonScene()->invalidate();
}

// send signal with the new typon name
void MYTypon::updateTyponName(bool clean){
    if ( clean )
        emit typonNameHasChanged(m_typonName );
    else
        emit typonNameHasChanged(m_typonName+ " *");
}

// center the scene after a little wait on first show
void MYTypon::center(){
    m_view->centerOn(0,0);
    m_view->centerOn(qApp->workSpace()->originPos(typonSize()));
}

void MYTypon::showEvent(QShowEvent *event){
    if ( m_firstShow ){
        m_firstShow = false;
        QTimer::singleShot(50, this, SLOT(center()));
    }
    QWidget::showEvent(event);
}

QString MYTypon::typonName(){
    return m_typonName;
}

void MYTypon::setTyponName(QString name){
    m_typonName = name;
}

QUndoStack *MYTypon::undoStack(){
    return m_undoStack;
}

MYTyponScene *MYTypon::typonScene(){
    return m_scene;
}

MYTyponView *MYTypon::typonView(){
    return m_view;
}

// set zoom to default value
void MYTypon::zoomReset(){
    m_view->resetMatrix();
    m_hRuler->resetMatrix();
    m_vRuler->resetMatrix();
    scale(0.25);
}

//zoom to the typon board
void MYTypon::zoomBoard(){
    QRectF typonRect = QRectF(0.0,0.0,typonSize().width(),typonSize().height());
    typonRect.translate(25000.0 - typonRect.width() / 2.0, 25000.0 - typonRect.height() / 2.0);
    zoomToRect(typonRect);
}

// zoom to fit a rect in the view
// maybe fitInView function should work as well????
void MYTypon::zoomToRect(QRectF rect, bool zoomIn){
    // get rect in scene coordinates
    QRectF r = m_view->mapFromScene(rect).boundingRect();
    QRectF viewportRect = m_view->viewport()->rect();

    double xratio = 0.0;
    double yratio = 0;

    // find the ratio in x and y according if we are zooming in or out
    if ( zoomIn ){
        xratio = viewportRect.width() / r.width();
        yratio = viewportRect.height() / r.height();
    } else {
        xratio = r.width() / viewportRect.width();
        yratio = r.height() / viewportRect.height();
    }
    //scale at the min ration in order to see the whole rect
    scale(qMin(xratio, yratio));
    //center the view on the new position and tell the view to update the lastMouse pos
    //(no mouse event actually occure)
    m_view->centerOn(rect.center());
    m_view->updateLastMousePos();
}

void MYTypon::zoomIn(){
    scale(2);
}

void MYTypon::zoomOut(){
    scale(0.5);
}

void MYTypon::zoomFit(){
    if ( qApp->currentTypon()->typonScene()->atLeastOneTyponItemIsVisible() )
        zoomToRect(qApp->currentTypon()->typonScene()->typonItemsBoundingRect());
    else
        zoomBoard();
}

void MYTypon::zoomSelection(){
    if ( qApp->currentTypon()->typonScene()->selectedTyponItems()->count() > 0 )
        zoomToRect(qApp->currentTypon()->typonScene()->selectedItemsBoundingRect());
}

// zoom to the new value
void MYTypon::scale(double scaleValue){
    double lvlOfDetails = m_view->levelOfdetails();
    double estimatedLevelOfDetails = scaleValue * lvlOfDetails;

    // find if new zoom value is inside defined boundaries
    // if not limit the zoom value
    if ( estimatedLevelOfDetails > 16 )
        scaleValue = 16 / lvlOfDetails;

    if ( estimatedLevelOfDetails < 0.015625 )
        scaleValue = 0.015625 / lvlOfDetails;

    // scale the view and the rulers
    m_view->scale(scaleValue,scaleValue);
    m_hRuler->scale(scaleValue,1);
    m_vRuler->scale(1,scaleValue);
    m_hRuler->horizontalScrollBar()->setValue(m_view->horizontalScrollBar()->value());
    m_vRuler->verticalScrollBar()->setValue(m_view->verticalScrollBar()->value());
    //tell the view to update the lastMouse pos (no mouse event actually occure)
    m_view->updateLastMousePos();
    // tell the main windos to enable or disable zoom actions according to new zoom value
    emit zoomWidgetsUpdateRequired();
}

double MYTypon::copperThickness(){
    return m_copperThickness;
}

void MYTypon::setCopperThickness(double thickness){
    m_copperThickness = thickness;
}

void MYTypon::hideRulers(bool hide){
   m_unitsLabel->setVisible(!hide);
   m_vRuler->setVisible(!hide);
   m_hRuler->setVisible(!hide);
}

// update the size of the current typon boarsd
void MYTypon::setTyponSize(QSizeF size){
    m_typonSize = size;
}

QSizeF MYTypon::typonSize(){
    return m_typonSize;
}

int MYTypon::instancesCounter = 0;
