#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QUndoView>
#include <QUndoGroup>
#include "global.h"
#include "mytabwidget.h"
#include "tools/mytoolbase.h"
#include "tools/MoveTool/movetool.h"
#include "utils/myunitslabel.h"
#include "./tools/SelectTool/selecttool.h"
#include "./tools/ZoomTool/zoomtool.h"
#include "./tools/MeasureTool/measuretool.h"
#include "./tools/TextTool/texttool.h"
#include "./tools/TrackTool/tracktool.h"
#include "./tools/DrawTool/drawtool.h"
#include "./tools/PadTool/padtool.h"
#include "./tools/ComponentTool/componenttool.h"
#include "layers/layersmenu.h"

class MYWorkSpace;
class MYOptions;
class MYTypon;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow(); 

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void loadWorkspaceFromFile(QString filePath);
    void loadDefaultWorkspace();
    void restoreFactoryWorkspace();
    void loadWorkspace();
    void saveWorkspace();
    void saveWorkspaceAs();
    void saveWorkspaceToFile(QString filePath);
    void setCurrentWorkSpaceAsDefault(bool warning = true);
    void showOptionsDLG();
    void workSpaceFileUpdated(QString fileName);
    void onCurrentTyponChange(MYTypon* typon);
    void on_actionGrid_triggered();
    void updateUndoActionText(QString text);
    void updateRedoActionText(QString text);
    void closeCurrentTypon();
    void updateStatusLabels(QPointF pos);
    void onToolChanged(int index);
    void onToolChanged(QAction *action);
    void onZoomIn();
    void onZoomOut();
    void onZoomWidgetUpdateRequired();
    void updateTools();
    void updateSelectionActions();
    void createGroup();
    void deleteGroup();
    void rotateLeft();
    void rotateRight();
    void rotateAngle();
    void horizontalMirror();
    void verticalMirror();
    void alignOnGrid();
    void sendToLayer(Layer *layer);
    void deleteSelection();
    void showTyponOptions();
    void onSave();
    void onSaveAs();
    void onOpen();
    void onRecentMenuUnfolded();

    void on_actiontest_triggered();

    void on_actiontest_2_triggered();

private:
    Ui::MainWindow *ui;

    MYWorkSpace *m_workspace;
    MYOptions *m_options;
    QLabel *m_workspaceLabel;
    MYTabWidget *m_centralWidget;
    QUndoGroup *m_undoGroup;
    QAction *m_undoAction;
    QAction *m_redoAction;
    MoveTool *m_moveTool;
    SelectTool *m_selectTool;
    ZoomTool *m_zoomTool;
    TrackTool *m_trackTool;
    PadTool *m_padTool;
    MeasureTool *m_measureTool;
    DrawTool *m_drawTool;
    TextTool *m_textTool;
    ComponentTool *m_componentTool;
    MYUnitsLabel *m_xLabel;
    MYUnitsLabel *m_yLabel;
    QActionGroup *m_toolsActionGroup;
    QAction *m_selectToolAction;
    QAction *m_moveToolAction;
    QAction *m_zoomToolAction;
    QAction *m_measureToolAction;
    QAction *m_trackToolAction;
    QAction *m_drawToolAction;
    QAction *m_padToolAction;
    QAction *m_textToolAction;
    QAction *m_componentToolAction;
    QLabel *m_zoomLabel;
    QMenu *m_menuDisplayToolbars;
    QMenu *m_menuDisplayTools;
    LayersMenu *m_layersMenu;
    QToolButton *m_layerButton;
    QMenu *m_recentMenu;

    void init();
    void initStatusBar();
    void loadOptions();
    void createUndoStuff();
    void connectActions();
    void createTools();
};

#endif // MAINWINDOW_H
