#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H

/* global singleton that keeps trace of pointers to utilities:
 *
 *
 *
 * */

#include <QApplication>
#include <QUndoGroup>
#include "workspace/myworkspace.h"
#include "myoptions.h"
#include "tools/mytoolbase.h"
#include "typonWidget/mytypon.h"
#include "tools/SelectTool/selectiontreewidget.h"
#include <QStatusBar>

class MYApplication : public QApplication
{
    Q_OBJECT

public:
    MYApplication(int &argc, char **argv);
    void setWorkSpace(MYWorkSpace *workSpace);
    MYWorkSpace *workSpace();
    void setOptions(MYOptions *options);
    MYOptions *options();
    void setMainTool(MYToolBase *tool);
    MYToolBase *mainTool();
    void setCurrentTypon(MYTypon *typon);
    MYTypon *currentTypon();
    void setUndoGroup(QUndoGroup *undoGroup);
    QUndoGroup *undoGroup();
    void setSelectionTree(SelectionTreeWidget *treeWidget);
    SelectionTreeWidget *selectionTree();
    QStatusBar *statusBar();
    void setStatusBar(QStatusBar *statusBar);

private:
    MYWorkSpace *m_workSpace;
    MYOptions *m_options;
    MYToolBase *m_mainTool;
    MYTypon *m_typon;
    QUndoGroup *m_undoGroup;
    SelectionTreeWidget *m_selectionTree;
    QStatusBar *m_statusBar;
};

#endif // MYAPPLICATION_H
