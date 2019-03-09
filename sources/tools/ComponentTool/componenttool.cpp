#include "componenttool.h"
#include "ui_componenttool.h"
#include "global.h"
#include "componentslistwidgetitem.h"
#include <QDebug>

ComponentTool::ComponentTool(QWidget *parent)
    : MYToolBase(parent), ui(new Ui::ComponentTool){
    ui->setupUi(this);
    setToolName(tr("Components"));
    setToolIcon(QIcon(":/pictures/chip2.png"));
    m_cursor = Qt::ArrowCursor;

    ui->searchLineEdit->addAction(QIcon(":/pictures/view.png"), QLineEdit::LeadingPosition);

    connect(ui->searchLineEdit,SIGNAL(returnPressed()),this,SLOT(updateFilter()));
    connect(qApp->workSpace(),SIGNAL(workspaceUpdated()),this,SLOT(updateFilter()));

    // make clear button emit returnPressed signal for search line edit
    // lack of signal for clearButton: bug submitted to Qt but nor resolved yet
    for (int i(0); i < ui->searchLineEdit->children().size(); ++i) {
        QAction *myClearAction(qobject_cast<QAction *>(ui->searchLineEdit->children().at(i)));
        if (myClearAction) {
            connect(myClearAction, &QAction::triggered,
                ui->searchLineEdit, &QLineEdit::returnPressed,
                Qt::QueuedConnection);
            break;
        }
    }
}

ComponentTool::~ComponentTool(){
    delete ui;
}

void ComponentTool::updateFilter(){
    if ( ui->searchLineEdit->text().isEmpty() ){
        ui->stackedWidget->setCurrentIndex(0);
    }else{
        ui->stackedWidget->setCurrentIndex(1);
        ui->listWidget->clear();

        foreach (QString path, qApp->workSpace()->librariesPaths) {
            QDirIterator it(path, QStringList() << "*"+ui->searchLineEdit->text()+"*.mtc", QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()){
                QString next = it.next();
                ComponentsListWidgetItem *listIt = new ComponentsListWidgetItem(ui->listWidget,1001,next);
                ui->listWidget->addItem(listIt);
            }
        }
        ui->listWidget->sortItems();
    }
}

void ComponentTool::handleMousePressEvent(QMouseEvent *event){
    MYToolBase::mousePressEventTool(event);
}

void ComponentTool::handleMouseReleaseEvent(QMouseEvent *event){
    MYToolBase::mouseReleaseEventTool(event);
}

void ComponentTool::handleMouseMoveEvent(QMouseEvent *event){
    MYToolBase::mouseMoveEventTool(event);
}

void ComponentTool::handleWheelEvent(QWheelEvent *event){
    MYToolBase::mouseWheelEventTool(event);
}

void ComponentTool::handleKeyPressEvent(QKeyEvent *event){
    MYToolBase::keyPressEventTool(event);
}

void ComponentTool::handleKeyReleaseEvent(QKeyEvent *event){
    MYToolBase::keyReleaseEventTool(event);
}

void ComponentTool::handleEnterEvent(QEvent *event){
    MYToolBase::enterEventTool(event);
}

void ComponentTool::handleLeaveEvent(QEvent *event){
    MYToolBase::leaveEventTool(event);
}
