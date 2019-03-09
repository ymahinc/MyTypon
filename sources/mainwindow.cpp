#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QScrollBar>
#include "workspace/myworkspace.h"
#include "myoptions.h"
#include "optionsdialog.h"
#include "layers/layerswidget.h"
#include "toolswidget/mytoolbox.h"
#include "angledialog.h"
#include "recentfileaction.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    init();
    // set size of toolbar icons
    ui->filesToolBar->setIconSize(QSize(16,16));
    ui->editToolBar->setIconSize(QSize(16,16));
    ui->optionsToolBar->setIconSize(QSize(16,16));
    ui->selectionToolBar->setIconSize(QSize(16,16));
}

MainWindow::~MainWindow(){
    delete ui;
}

// init mainwindow environment
void MainWindow::init(){
    qApp->setStatusBar(ui->statusBar);

    // create new worspace and update global pointer
    m_workspace = new MYWorkSpace(this);
    qApp->setWorkSpace(m_workspace);

    // create new options and update global pointer
    m_options = new MYOptions(this);
    qApp->setOptions(m_options);

    // read last saved geometry of mainwindow and apply if exists
    QSettings options("mytypon.ini", QSettings::IniFormat);
    QByteArray geometry = options.value("geometry").toByteArray();
    if ( ! geometry.isEmpty() )
        restoreGeometry(geometry);

    initStatusBar();    
    createUndoStuff();

    m_recentMenu = new QMenu(this);
    m_recentMenu->setTitle(tr("Recent files"));
    ui->menuFile->insertMenu(ui->menuWorkspaces->menuAction(),m_recentMenu);
    ui->menuFile->insertSeparator(ui->menuWorkspaces->menuAction());

    m_layersMenu = new LayersMenu(this);
    ui->menuSelection->insertMenu(ui->actionAlign_on_grid,m_layersMenu);
    ui->menuSelection->insertSeparator(ui->actionAlign_on_grid);

    m_layerButton = new QToolButton(this);
    m_layerButton->setMenu(m_layersMenu);
    m_layerButton->setIcon(QIcon(":/pictures/layers_map.png"));
    m_layerButton->setToolTip(tr("Send to layer"));
    m_layerButton->setPopupMode(QToolButton::InstantPopup);
    ui->selectionToolBar->insertWidget(ui->actionAlign_on_grid,m_layerButton);

    //set central tabbed widget
    m_centralWidget = new MYTabWidget(this);
    setCentralWidget(m_centralWidget);

    connectActions();
    loadOptions();
    loadDefaultWorkspace();
    createTools();

    // create new empty typon
    m_centralWidget->addNewTypon();

    m_trackTool->setMode(TrackTool::OneAngle1);
}

// init status bar labels
void MainWindow::initStatusBar(){
    m_workspaceLabel = new QLabel(this);
    ui->statusBar->addWidget(m_workspaceLabel);
    m_xLabel = new MYUnitsLabel(this);
    m_xLabel->setPrefix(tr("x:"));
    ui->statusBar->addWidget(m_xLabel);
    m_yLabel = new MYUnitsLabel(this);
    m_yLabel->setPrefix(tr("y:"));
    ui->statusBar->addWidget(m_yLabel);
    m_zoomLabel = new QLabel(this);
    m_zoomLabel->setText(tr("Zoom: 1.0"));
    ui->statusBar->addWidget(m_zoomLabel);
}

// create application tools
void MainWindow::createTools(){
    // create action group for tools menu
    m_toolsActionGroup = new QActionGroup(this);

    // create tools and at it to the toolWidget

    m_selectTool = new SelectTool(this, ui->menuSelection);
    ui->toolBoxWidget->addTool(m_selectTool);
    m_selectToolAction = new QAction(m_selectTool->toolIcon(),m_selectTool->toolName(),this);
    m_selectToolAction->setCheckable(true);
    m_selectToolAction->setData(0);
    m_toolsActionGroup->addAction(m_selectToolAction);
    connect(m_selectTool,SIGNAL(showOptionsSignal()),this,SLOT(showTyponOptions()));

    m_moveTool = new MoveTool(this);
    ui->toolBoxWidget->addTool(m_moveTool);
    m_moveToolAction = new QAction(m_moveTool->toolIcon(),m_moveTool->toolName(),this);
    m_moveToolAction->setCheckable(true);
    m_moveToolAction->setData(1);
    m_toolsActionGroup->addAction(m_moveToolAction);

    m_zoomTool = new ZoomTool(this);
    ui->toolBoxWidget->addTool(m_zoomTool);
    m_zoomToolAction = new QAction(m_zoomTool->toolIcon(),m_zoomTool->toolName(),this);
    m_zoomToolAction->setCheckable(true);
    m_zoomToolAction->setData(2);
    m_toolsActionGroup->addAction(m_zoomToolAction);

    m_trackTool = new TrackTool(this);
    ui->toolBoxWidget->addTool(m_trackTool);
    m_trackToolAction = new QAction(m_trackTool->toolIcon(),m_trackTool->toolName(),this);
    m_trackToolAction->setCheckable(true);
    m_trackToolAction->setData(3);
    m_toolsActionGroup->addAction(m_trackToolAction);

    m_drawTool = new DrawTool(this);
    ui->toolBoxWidget->addTool(m_drawTool);
    m_drawToolAction = new QAction(m_drawTool->toolIcon(),m_drawTool->toolName(),this);
    m_trackToolAction->setCheckable(true);
    m_trackToolAction->setData(4);
    m_toolsActionGroup->addAction(m_trackToolAction);

    m_padTool = new PadTool(this);
    ui->toolBoxWidget->addTool(m_padTool);
    m_padToolAction = new QAction(m_padTool->toolIcon(),m_padTool->toolName(),this);
    m_padToolAction->setCheckable(true);
    m_padToolAction->setData(5);
    m_toolsActionGroup->addAction(m_padToolAction);

    m_textTool = new TextTool(this);
    ui->toolBoxWidget->addTool(m_textTool);
    m_textToolAction = new QAction(m_textTool->toolIcon(),m_textTool->toolName(),this);
    m_textToolAction->setCheckable(true);
    m_textToolAction->setData(6);
    m_toolsActionGroup->addAction(m_textToolAction);

    m_measureTool = new MeasureTool(this);
    ui->toolBoxWidget->addTool(m_measureTool);
    m_measureToolAction = new QAction(m_measureTool->toolIcon(),m_measureTool->toolName(),this);
    m_measureToolAction->setCheckable(true);
    m_measureToolAction->setData(7);
    m_toolsActionGroup->addAction(m_measureToolAction);

    m_componentTool = new ComponentTool(this);
    ui->toolBoxWidget->addTool(m_componentTool);
    m_componentToolAction = new QAction(m_componentTool->toolIcon(),m_componentTool->toolName(),this);
    m_componentToolAction->setCheckable(true);
    m_componentToolAction->setData(8);
    m_toolsActionGroup->addAction(m_componentToolAction);

    ui->menutools->addActions(m_toolsActionGroup->actions());
    m_selectToolAction->setChecked(true);
    ui->toolBoxWidget->setCurrentIndex(0);

    // update global tool pointer
    qApp->setMainTool(m_selectTool);

    // connect tool change from toolwidget or menu to update corresponding selected tool
    connect(ui->toolBoxWidget,SIGNAL(currentChanged(int)),this,SLOT(onToolChanged(int)));
    connect(m_toolsActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(onToolChanged(QAction*)));
}

void MainWindow::showTyponOptions(){
    OptionsDialog *optionsdDLG = new OptionsDialog(this,"1");
    if ( optionsdDLG->exec() == QDialog::Accepted ){
        m_trackTool->updateCurrentMax();
    }
}

// tool has been changed from menu, update toolwidget
void MainWindow::onToolChanged(QAction *action){
    ui->toolBoxWidget->setCurrentIndex(action->data().toInt());
}

//tool has been changed from toolwidget, update menu
void MainWindow::onToolChanged(int index){
    //disconnect tools action signal to avoid infinite loop
    disconnect(m_toolsActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(onToolChanged(QAction*)));

    // cast the tool at index and select corresponding one, update global tool pointer
    SelectTool *selectT = qobject_cast<SelectTool *>(ui->toolBoxWidget->widget(index));
    if ( selectT ){
        m_selectToolAction->setChecked(true);
        qApp->setMainTool(m_selectTool);
        updateSelectionActions();
    }else{
        ui->actionSelectAll->setEnabled(false);
        ui->actionDelete->setEnabled(false);
        ui->actionClearSelection->setEnabled(false);
        ui->actionRotate_angle->setEnabled(false);
        ui->actionRotate_left->setEnabled(false);
        ui->actionRotate_Right->setEnabled(false);
        ui->actionHorizontal_mirror->setEnabled(false);
        ui->actionVertical_mirror->setEnabled(false);
        ui->actionGenerate_component->setEnabled(false);
        ui->actionAlign_on_grid->setEnabled(false);
        ui->actionGroup->setEnabled(false);
        ui->actionUngroup->setEnabled(false);
    }

    MoveTool *moveT = qobject_cast<MoveTool *>(ui->toolBoxWidget->widget(index));
    if ( moveT ){
        m_moveToolAction->setChecked(true);
        qApp->setMainTool(m_moveTool);
    }

    ZoomTool *zoomT = qobject_cast<ZoomTool *>(ui->toolBoxWidget->widget(index));
    if ( zoomT ){
        m_zoomToolAction->setChecked(true);
        qApp->setMainTool(m_zoomTool);
    }

    TrackTool *trackT = qobject_cast<TrackTool *>(ui->toolBoxWidget->widget(index));
    if ( trackT ){
        m_trackToolAction->setChecked(true);
        qApp->setMainTool(m_trackTool);
    }

    DrawTool *drawT = qobject_cast<DrawTool *>(ui->toolBoxWidget->widget(index));
    if ( drawT ){
        m_trackToolAction->setChecked(true);
        qApp->setMainTool(m_drawTool);
    }

    PadTool *padT = qobject_cast<PadTool *>(ui->toolBoxWidget->widget(index));
    if ( padT ){
        m_padToolAction->setChecked(true);
        qApp->setMainTool(m_padTool);
    }

    TextTool *textT = qobject_cast<TextTool *>(ui->toolBoxWidget->widget(index));
    if ( textT ){
        m_textToolAction->setChecked(true);
        qApp->setMainTool(m_textTool);
    }

    MeasureTool *measureT = qobject_cast<MeasureTool *>(ui->toolBoxWidget->widget(index));
    if ( measureT ){
        m_measureToolAction->setChecked(true);
        qApp->setMainTool(m_measureTool);
    }

    ComponentTool *componentT = qobject_cast<ComponentTool *>(ui->toolBoxWidget->widget(index));
    if ( componentT ){
        m_componentToolAction->setChecked(true);
        qApp->setMainTool(m_componentTool);
    }

    // update cursor according to current tool on every open typon
    for (int i = 0; i < m_centralWidget->count(); i++)
        m_centralWidget->typonAt(i)->typonView()->setCursor(qApp->mainTool()->cursor());

    //reconnect menu tool action signals
    connect(m_toolsActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(onToolChanged(QAction*)));
}

// create undogroup and related actions, connect internal undo/redo signals
// then update global pointer
void MainWindow::createUndoStuff(){
    m_undoGroup = new QUndoGroup(this);
    m_undoAction = m_undoGroup->createUndoAction(this,tr("Undo"));
    m_undoAction->setIcon(QIcon(":/pictures/undo.png"));
    m_undoAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+Z", 0));
    m_redoAction = m_undoGroup->createRedoAction(this,tr("Redo"));
    m_redoAction->setIcon(QIcon(":/pictures/redo.png"));
    m_redoAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+Z", 0));
    ui->editToolBar->insertAction(ui->actionCopy,m_undoAction);
    ui->editToolBar->insertAction(ui->actionCopy,m_redoAction);
    ui->editToolBar->insertSeparator(ui->actionCopy);
    ui->menuEdit->insertAction(ui->actionCopy,m_undoAction);
    ui->menuEdit->insertAction(ui->actionCopy,m_redoAction);
    ui->menuEdit->insertSeparator(ui->actionCopy);
    ui->undoWidget->undoView()->setGroup(m_undoGroup);
    connect(m_undoGroup,SIGNAL(undoTextChanged(QString)),this,SLOT(updateUndoActionText(QString)));
    connect(m_undoGroup,SIGNAL(redoTextChanged(QString)),this,SLOT(updateRedoActionText(QString)));
    qApp->setUndoGroup(m_undoGroup);
}

void MainWindow::updateUndoActionText(QString text){
    m_undoAction->setText(tr("undo \"%1\"").arg(text));
}

void MainWindow::updateRedoActionText(QString text){
    m_redoAction->setText(tr("redo \"%1\"").arg(text));
}

// create diplay menu and connect signals
void MainWindow::connectActions(){
    m_menuDisplayToolbars = new QMenu(tr("Toolbars"),this);
    m_menuDisplayTools = new QMenu(tr("Tools"),this);

    m_menuDisplayToolbars->addAction(ui->filesToolBar->toggleViewAction());
    m_menuDisplayToolbars->addAction(ui->editToolBar->toggleViewAction());
    m_menuDisplayToolbars->addAction(ui->selectionToolBar->toggleViewAction());
    m_menuDisplayToolbars->addAction(ui->optionsToolBar->toggleViewAction());

    m_menuDisplayTools->addAction(ui->toolsDockWidget->toggleViewAction());
    m_menuDisplayTools->addAction(ui->layersDockWidget->toggleViewAction());
    m_menuDisplayTools->addAction(ui->undoDockWidget->toggleViewAction());
    m_menuDisplayTools->addAction(ui->miniViewDockWidget->toggleViewAction());

    ui->menuWindows->addMenu(m_menuDisplayToolbars);
    ui->menuWindows->addMenu(m_menuDisplayTools);

    connect(ui->actionRestoreFactoryWSP,SIGNAL(triggered(bool)),this,SLOT(restoreFactoryWorkspace()));
    connect(ui->actionLoadDefaultWSP,SIGNAL(triggered(bool)),this,SLOT(loadDefaultWorkspace()));
    connect(ui->actionLoadWSPFromFile,SIGNAL(triggered(bool)),this,SLOT(loadWorkspace()));
    connect(ui->actionSaveWSP,SIGNAL(triggered(bool)),this,SLOT(saveWorkspace()));
    connect(ui->actionSaveWSPAs,SIGNAL(triggered(bool)),this,SLOT(saveWorkspaceAs()));
    connect(ui->actionSetCurrentWSPAsDefault,SIGNAL(triggered(bool)),this,SLOT(setCurrentWorkSpaceAsDefault()));
    connect(ui->actionGeneralOptions,SIGNAL(triggered(bool)),this,SLOT(showOptionsDLG()));
    connect(qApp->workSpace(),SIGNAL(workspaceFileChanged(QString)),this,SLOT(workSpaceFileUpdated(QString)));
    connect(ui->actionNewFile,SIGNAL(triggered(bool)),m_centralWidget,SLOT(addNewTypon()));
    connect(m_centralWidget,SIGNAL(currentTyponHasChanged(MYTypon*)),this,SLOT(onCurrentTyponChange(MYTypon*)));
    connect(ui->actionCloseFile,SIGNAL(triggered(bool)),this,SLOT(closeCurrentTypon()));
    connect(qApp->workSpace(),SIGNAL(workspaceUnitChanged()),m_xLabel,SLOT(onUnitChanged()));
    connect(qApp->workSpace(),SIGNAL(workspaceUnitChanged()),m_yLabel,SLOT(onUnitChanged()));
    connect(m_undoGroup,SIGNAL(cleanChanged(bool)),ui->actionSave,SLOT(setDisabled(bool)));
    connect(ui->actionGroup,SIGNAL(triggered(bool)),this,SLOT(createGroup()));
    connect(ui->actionUngroup,SIGNAL(triggered(bool)),this,SLOT(deleteGroup()));
    connect(ui->actionRotate_left,SIGNAL(triggered(bool)),this,SLOT(rotateLeft()));
    connect(ui->actionRotate_Right,SIGNAL(triggered(bool)),this,SLOT(rotateRight()));
    connect(ui->actionRotate_angle,SIGNAL(triggered(bool)),this,SLOT(rotateAngle()));
    connect(ui->actionHorizontal_mirror,SIGNAL(triggered(bool)),this,SLOT(horizontalMirror()));
    connect(ui->actionVertical_mirror,SIGNAL(triggered(bool)),this,SLOT(verticalMirror()));
    connect(ui->actionAlign_on_grid,SIGNAL(triggered(bool)),this,SLOT(alignOnGrid()));
    connect(m_layersMenu,SIGNAL(sendToLayer(Layer*)),this,SLOT(sendToLayer(Layer*)));
    connect(ui->actionDelete,SIGNAL(triggered(bool)),this,SLOT(deleteSelection()));
    connect(ui->actionSave,SIGNAL(triggered(bool)),this,SLOT(onSave()));
    connect(ui->actionSaveAs,SIGNAL(triggered(bool)),this,SLOT(onSaveAs()));
    connect(ui->actionOpenFile,SIGNAL(triggered(bool)),this,SLOT(onOpen()));
    connect(m_recentMenu,SIGNAL(aboutToShow()),this,SLOT(onRecentMenuUnfolded()));
}

void MainWindow::deleteSelection(){
    qApp->currentTypon()->typonScene()->deleteSelection();
}

void MainWindow::alignOnGrid(){
    qApp->currentTypon()->typonScene()->alignSelectedOnGrid();
}

void MainWindow::sendToLayer(Layer *layer){
    qApp->currentTypon()->typonScene()->sendSelectedToLayer(layer);
}

void MainWindow::createGroup(){
    if ( qApp->currentTypon() )
        qApp->currentTypon()->typonScene()->createGroupFromSelection();
}

void MainWindow::deleteGroup(){
    if ( qApp->currentTypon() )
        qApp->currentTypon()->typonScene()->deleteGroupsFromSelection();
}

void MainWindow::rotateLeft(){
    if ( qApp->currentTypon() )
        qApp->currentTypon()->typonScene()->rotateSelectionLeft();
}

void MainWindow::rotateRight(){
    if ( qApp->currentTypon() )
        qApp->currentTypon()->typonScene()->rotateSelectionRight();
}

void MainWindow::rotateAngle(){
    if ( qApp->currentTypon() ){
        AngleDialog *dlg = new AngleDialog(this);
        if ( dlg->exec() == QDialog::Accepted )
            qApp->currentTypon()->typonScene()->rotateSelection(dlg->getAngle());
    }
}

void MainWindow::horizontalMirror(){
    if ( qApp->currentTypon() )
        qApp->currentTypon()->typonScene()->horizontalMirrorSelection();
}

void MainWindow::verticalMirror(){
    if ( qApp->currentTypon() )
        qApp->currentTypon()->typonScene()->verticalMirrorSelection();
}

// set tools enabled or disabled depending of the current layer accessibility
void MainWindow::updateTools(){
    if ( !qApp->currentTypon()->typonScene()->layers()->activeLayer()->isVisible() ||
         !qApp->currentTypon()->typonScene()->layers()->activeLayer()->isEnabled() ){
        m_trackTool->setEnabled(false);
        m_drawTool->setEnabled(false);
        m_padTool->setEnabled(false);
        m_textTool->setEnabled(false);
    }else{
        m_trackTool->setEnabled(true);
        m_drawTool->setEnabled(true);
        m_padTool->setEnabled(true);
        m_textTool->setEnabled(true);
    }
    m_trackTool->updateCurrentMax();
}

void MainWindow::closeEvent(QCloseEvent *event){
    // try to close all opens typons, cancel event if user cancel save/discard on modified one

    QVector<MYTypon *> v;
    for (int i = 0 ; i < m_centralWidget->count(); i ++ )
        v.append(m_centralWidget->typonAt(i));
    for (int i = 0; i < v.count(); i++ ){
        int ret = m_centralWidget->closeTypon(v.at(i), true);
        if ( ret == -1 ){
            event->ignore();
            return;
        }
    }

    if ( qApp->options()->saveWorkSpaceOnQuit )
        setCurrentWorkSpaceAsDefault(false);
    QSettings settings("mytypon.ini", QSettings::IniFormat);
    settings.setValue("geometry", saveGeometry());
    qApp->options()->save();
}

void MainWindow::updateStatusLabels(QPointF pos){
    m_xLabel->setRawValue(pos.x() - qApp->workSpace()->originPos(qApp->currentTypon()->typonSize()).x());
    m_yLabel->setRawValue(pos.y() - qApp->workSpace()->originPos(qApp->currentTypon()->typonSize()).y());
}

void MainWindow::onCurrentTyponChange(MYTypon* typon){
    if ( qApp->currentTypon() ){
        disconnect(m_zoomTool,SIGNAL(zoomIn()),qApp->currentTypon(),SLOT(zoomIn()));
        disconnect(m_zoomTool,SIGNAL(zoomOut()),qApp->currentTypon(),SLOT(zoomOut()));
        disconnect(m_zoomTool,SIGNAL(zoomBoard()),qApp->currentTypon(),SLOT(zoomBoard()));
        disconnect(m_zoomTool,SIGNAL(zoomReset()),qApp->currentTypon(),SLOT(zoomReset()));
        disconnect(m_zoomTool,SIGNAL(zoomFit()),qApp->currentTypon(),SLOT(zoomFit()));
        disconnect(m_zoomTool,SIGNAL(zoomSelection()),qApp->currentTypon(),SLOT(zoomSelection()));
        disconnect(m_zoomTool,SIGNAL(zoomRect(QRectF,bool)),qApp->currentTypon(),SLOT(zoomToRect(QRectF,bool)));
        disconnect(ui->actionZoomIn,SIGNAL(triggered(bool)),qApp->currentTypon(),SLOT(zoomIn()));
        disconnect(ui->actionZoomOut,SIGNAL(triggered(bool)),qApp->currentTypon(),SLOT(zoomOut()));
    }
    qApp->setCurrentTypon(typon);
    m_trackTool->updateCurrentMax();
    ui->layersWidget->setLayersStack(qApp->currentTypon()->typonScene()->layers());
    qApp->undoGroup()->setActiveStack(qApp->currentTypon()->undoStack());
    connect(qApp->currentTypon()->typonScene()->layers(),SIGNAL(layersStackUpdated()),this,SLOT(updateTools()));
    connect(qApp->currentTypon()->typonScene()->layers(),SIGNAL(layerUpdated(Layer*)),this,SLOT(updateTools()));
    connect(qApp->currentTypon()->typonScene()->layers(),SIGNAL(activeLayerChanged()),this,SLOT(updateTools()));
    connect(qApp->currentTypon()->typonView(),SIGNAL(mousePosHasChangedScenePos(QPointF)),this,SLOT(updateStatusLabels(QPointF)));
    connect(qApp->currentTypon()->typonView(),SIGNAL(viewportLeaved()),m_xLabel,SLOT(clearText()));
    connect(qApp->currentTypon()->typonView(),SIGNAL(viewportLeaved()),m_yLabel,SLOT(clearText()));
    connect(qApp->currentTypon(),SIGNAL(zoomWidgetsUpdateRequired()),this,SLOT(onZoomWidgetUpdateRequired()));
    connect(qApp->currentTypon(),SIGNAL(zoomWidgetsUpdateRequired()),m_zoomTool,SLOT(updateWidgets()));
    connect(qApp->currentTypon()->typonView()->horizontalScrollBar(),SIGNAL(valueChanged(int)),ui->miniViewWidget,SLOT(invalidate()));
    connect(qApp->currentTypon()->typonView()->verticalScrollBar(),SIGNAL(valueChanged(int)),ui->miniViewWidget,SLOT(invalidate()));
    connect(m_zoomTool,SIGNAL(zoomIn()),qApp->currentTypon(),SLOT(zoomIn()));
    connect(m_zoomTool,SIGNAL(zoomOut()),qApp->currentTypon(),SLOT(zoomOut()));
    connect(ui->actionZoomIn,SIGNAL(triggered(bool)),qApp->currentTypon(),SLOT(zoomIn()));
    connect(ui->actionZoomOut,SIGNAL(triggered(bool)),qApp->currentTypon(),SLOT(zoomOut()));
    connect(m_zoomTool,SIGNAL(zoomBoard()),qApp->currentTypon(),SLOT(zoomBoard()));
    connect(m_zoomTool,SIGNAL(zoomReset()),qApp->currentTypon(),SLOT(zoomReset()));
    connect(m_zoomTool,SIGNAL(zoomRect(QRectF,bool)),qApp->currentTypon(),SLOT(zoomToRect(QRectF,bool)));
    connect(m_zoomTool,SIGNAL(zoomFit()),qApp->currentTypon(),SLOT(zoomFit()));
    connect(m_zoomTool,SIGNAL(zoomSelection()),qApp->currentTypon(),SLOT(zoomSelection()));
    connect(ui->actionSelectAll,SIGNAL(triggered(bool)),qApp->currentTypon()->typonScene(),SLOT(selectAll()));
    connect(ui->actionClearSelection,SIGNAL(triggered(bool)),qApp->currentTypon()->typonScene(),SLOT(clearSelection()));
    connect(qApp->currentTypon()->typonScene(),SIGNAL(typonItemsSelectionChanged()),this,SLOT(updateSelectionActions()));
    connect(ui->actionCopy,SIGNAL(triggered(bool)),qApp->currentTypon(),SLOT(copy()));
    connect(ui->actionPaste,SIGNAL(triggered(bool)),qApp->currentTypon(),SLOT(paste()));
    m_selectTool->updateList();
    connect(qApp->currentTypon()->typonScene(),SIGNAL(typonItemsSelectionChanged()),m_selectTool,SLOT(updateList()));
    onZoomWidgetUpdateRequired();
    m_zoomTool->updateWidgets();
    ui->miniViewWidget->updateTypon();
    updateTools();
    updateSelectionActions();
}

void MainWindow::updateSelectionActions(){
    ui->actionSelectAll->setEnabled(true);
    ui->actionDelete->setEnabled(true);
    ui->actionClearSelection->setEnabled(true);
    ui->actionRotate_angle->setEnabled(true);
    ui->actionRotate_left->setEnabled(true);
    ui->actionRotate_Right->setEnabled(true);
    ui->actionHorizontal_mirror->setEnabled(true);
    ui->actionVertical_mirror->setEnabled(true);
    ui->actionGenerate_component->setEnabled(true);
    ui->actionAlign_on_grid->setEnabled(true);
    ui->actionGroup->setEnabled(true);
    ui->actionUngroup->setEnabled(true);
    m_layersMenu->setEnabled(true);
    m_layerButton->setEnabled(true);
    ui->actionCopy->setEnabled(true);

    if ( qApp->currentTypon()->typonScene()->selectedTyponItems()->count() == 0 ){
        ui->actionDelete->setEnabled(false);
        ui->actionClearSelection->setEnabled(false);
        ui->actionRotate_angle->setEnabled(false);
        ui->actionRotate_left->setEnabled(false);
        ui->actionRotate_Right->setEnabled(false);
        ui->actionHorizontal_mirror->setEnabled(false);
        ui->actionVertical_mirror->setEnabled(false);
        ui->actionGenerate_component->setEnabled(false);
        ui->actionAlign_on_grid->setEnabled(false);
        ui->actionGroup->setEnabled(false);
        ui->actionUngroup->setEnabled(false);
        m_layersMenu->setEnabled(false);
        m_layerButton->setEnabled(false);
        ui->actionCopy->setEnabled(false);

    }
    if ( qApp->currentTypon()->typonScene()->selectedTyponItems()->count() == 1 )
        ui->actionGroup->setEnabled(false);
}

void MainWindow::closeCurrentTypon(){
    m_centralWidget->closeTypon(qApp->currentTypon());
}

void MainWindow::onZoomIn(){
    qApp->currentTypon()->scale(2);
}

void MainWindow::onZoomOut(){
    qApp->currentTypon()->scale(0.5);
}

void MainWindow::onZoomWidgetUpdateRequired(){
    ui->actionZoomIn->setEnabled(qApp->currentTypon()->typonView()->canZoomIn());
    ui->actionZoomOut->setEnabled(qApp->currentTypon()->typonView()->canZoomOut());
    m_zoomLabel->setText(tr("Zoom: %1").arg(qApp->currentTypon()->typonView()->levelOfdetails()*4.0));
}

#define REGION_OPTIONS {

// load options from global pointer
void MainWindow::loadOptions(){
    qApp->options()->load();
}

// show options dialog and select is first page (general options)
void MainWindow::showOptionsDLG(){
    OptionsDialog *optionsdDLG = new OptionsDialog(this,"0");
    if ( optionsdDLG->exec() == QDialog::Accepted ){
        m_trackTool->updateCurrentMax();
    }
}

// show options dialog and select is third page (grid options)
void MainWindow::on_actionGrid_triggered(){
    OptionsDialog *optionsdDLG = new OptionsDialog(this,"2");
    if ( optionsdDLG->exec() == QDialog::Accepted ){
        m_trackTool->updateCurrentMax();
    }
}

#define REGION_OPTIONS_END }

#define REGION_WORKSPACE {

// load workspace from file selected by iser
void MainWindow::loadWorkspace(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open workspace file"), "", tr("Workspace Files (*.wsp)"));
    if ( ! fileName.isEmpty() )
        loadWorkspaceFromFile(fileName);
}

// restore workspace values from factory resources
void MainWindow::restoreFactoryWorkspace(){
    int ret = QMessageBox::warning(this, tr("Restore factory workspace"),
                                   tr("You are about to restore factory workspace settings.\n"
                                      "Are you sure?"), QMessageBox::Yes | QMessageBox::No);
    if ( ret == QMessageBox::Yes ){
        qApp->workSpace()->restoreFactorySettings();
        restoreState(qApp->workSpace()->widgetsState());
    }
}

// update workspace with workspace file values
void MainWindow::loadWorkspaceFromFile(QString filePath){
    qApp->workSpace()->loadFromFile(filePath);
    // restore the widgets geomtry
    restoreState(qApp->workSpace()->widgetsState());
}

//load default workspace, if default workspace file do not exists, create it with factory resources values
// and then load it
void MainWindow::loadDefaultWorkspace(){
    if ( ! QFile::exists("defaultworkspace.wsp") ) {
       qApp->workSpace()->restoreFactorySettings();
       qApp->workSpace()->writeToFile("defaultworkspace.wsp");
    }
    loadWorkspaceFromFile("defaultworkspace.wsp");
}

// save current workspace to current workspace file
void MainWindow::saveWorkspace(){
    saveWorkspaceToFile(qApp->workSpace()->currentWSPFilePath);
}

// save current workspace to file
void MainWindow::saveWorkspaceAs(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save workspace file"), "", tr("Workspace Files (*.wsp)"));
    if ( ! fileName.isEmpty() )
        saveWorkspaceToFile(fileName);
}

// save current workspace to file passed as parameter
void MainWindow::saveWorkspaceToFile(QString filePath){
    qApp->workSpace()->writeToFile(filePath,saveState());
}

// save current workspace in deault workspace file
void MainWindow::setCurrentWorkSpaceAsDefault(bool warning){
    bool save = false;
    if ( warning ){
        int ret = QMessageBox::warning(this, tr("Overwrite default workspace"),
                                       tr("You are about to modifiy default workspace settings.\n"
                                          "Are you sure?"), QMessageBox::Yes | QMessageBox::No);
        if ( ret == QMessageBox::Yes )
            save = true;
    }
    if ( save || !warning ) {
        qApp->workSpace()->currentWSPFilePath = "defaultworkspace.wsp";
        saveWorkspaceToFile("defaultworkspace.wsp");
    }
}

// update status bar label if workspace file has changed
void MainWindow::workSpaceFileUpdated(QString fileName){
    m_workspaceLabel->setText(tr("Workspace: %1").arg(fileName));
}

#define REGION_WORKSPACE_END }

void MainWindow::onSave(){
    m_centralWidget->save(qApp->currentTypon());
}

void MainWindow::onSaveAs(){
    m_centralWidget->save(qApp->currentTypon(),true);
}

void MainWindow::onOpen(){
    QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open Typon"), qApp->options()->lastOpenedPath, tr("Typon Files (*.myt)"));

    if ( ! fileName.isEmpty() ){
        QFileInfo fileInfo(fileName);
        qApp->options()->lastOpenedPath = fileInfo.absolutePath();
        m_centralWidget->openFile(fileName);
    }
}

void MainWindow::onRecentMenuUnfolded(){
    m_recentMenu->clear();
    QSettings settings("recentfiles", QSettings::IniFormat);
    QStringList recentFilePaths =
            settings.value("recentFiles").toStringList();
    foreach (QString path, recentFilePaths) {
        RecentFileAction *action = new RecentFileAction(path,m_recentMenu);
        m_recentMenu->addAction(action);
        connect(action,SIGNAL(openFileTriggered(QString)),m_centralWidget,SLOT(openFile(QString)));
    }
}


#include "tools/TextTool/addtextundocommand.h"

void MainWindow::on_actiontest_triggered(){
    QFile file("test.myt");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);   // we will serialize the data into the file
    foreach (QGraphicsItem *it, qApp->currentTypon()->typonScene()->items()) {
        TextItem *textIt = dynamic_cast<TextItem *>(it);
        if ( textIt)
            textIt->save(out);
    }
}

void MainWindow::on_actiontest_2_triggered(){
    QFile file("test.myt");
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);    // read the data serialized from the file
    TextItem *textIt = new TextItem(0);
    textIt->restore(in);
    //textIt->setParentItem(qApp->currentTypon()->typonScene()->layers()->activeLayer());
    AddTextUndoCommand *txtCmd = new AddTextUndoCommand(textIt);
    qApp->undoGroup()->activeStack()->push(txtCmd);
    delete textIt;
}
