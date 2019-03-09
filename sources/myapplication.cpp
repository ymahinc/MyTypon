#include "myapplication.h"
#include <QDebug>

MYApplication::MYApplication(int &argc, char **argv)
    : QApplication(argc,argv){
    m_typon = 0;
}

QStatusBar *MYApplication::statusBar(){
    return m_statusBar;
}

void MYApplication::setStatusBar(QStatusBar *statusBar){
    m_statusBar = statusBar;
}

// pointer to the current workspace
void MYApplication::setWorkSpace(MYWorkSpace *workSpace){
    m_workSpace = workSpace;
}

MYWorkSpace *MYApplication::workSpace(){
    return m_workSpace;
}

// pointer to the current options
void MYApplication::setOptions(MYOptions *options){
    m_options = options;
}

MYOptions *MYApplication::options(){
    return m_options;
}

// pointer to the current used tool
void MYApplication::setMainTool(MYToolBase *tool){
    m_mainTool = tool;
}

MYToolBase *MYApplication::mainTool(){
    return m_mainTool;
}

// pointer to the current typon
// may be 0 at startup to avoid tools to use an invalid pointer as they are created before first typon
void MYApplication::setCurrentTypon(MYTypon *typon){
    m_typon = typon;
}

MYTypon *MYApplication::currentTypon(){
    return m_typon;
}

// pointer to the application undogroup
void MYApplication::setUndoGroup(QUndoGroup *undoGroup){
    m_undoGroup = undoGroup;
}

QUndoGroup *MYApplication::undoGroup(){
    return m_undoGroup;
}

// pointer to the selection tree widget, tree items need update when undo/redo command are applied to selected items
void MYApplication::setSelectionTree(SelectionTreeWidget *treeWidget){
    m_selectionTree = treeWidget;
}

SelectionTreeWidget *MYApplication::selectionTree(){
    return m_selectionTree;
}
