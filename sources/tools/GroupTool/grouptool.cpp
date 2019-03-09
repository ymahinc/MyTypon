#include "grouptool.h"
#include "ui_grouptool.h"
#include "editgroupundocommand.h"
#include "global.h"
#include "items/myitembase.h"

GroupTool::GroupTool(QWidget *parent, MYItemBase *item)
    : MYToolBase(parent), ui(new Ui::GroupTool){
    ui->setupUi(this);

    m_group = item;
    ui->tableWidget->setCellWidget(0,1,ui->nameLineEdit);

    // set a pointer to this on the item so we can update widgets values when item properties change
    m_group->setGroupTool(this);

    updateGroup();

    connect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged()));
}

GroupTool::~GroupTool(){
    delete ui;
    m_group->setGroupTool(0);
}

// change name value, can only occured on an existing item on the scene
void GroupTool::onNameChanged(){
    EditGroupUndoCommand *textGroupCommand = new EditGroupUndoCommand(m_group,ui->nameLineEdit->text());
    qApp->undoGroup()->activeStack()->push(textGroupCommand);
    emit nameChanged(m_group);
}

void GroupTool::updateGroup(){
    disconnect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged()));
    ui->nameLineEdit->setText(m_group->itemName());
    emit nameChanged(m_group);
    connect(ui->nameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged()));
}

void GroupTool::handleMousePressEvent(QMouseEvent *event){
    Q_UNUSED(event)
}

void GroupTool::handleMouseReleaseEvent(QMouseEvent *event){
    Q_UNUSED(event)
}

void GroupTool::handleMouseMoveEvent(QMouseEvent *event){
    Q_UNUSED(event)
}

void GroupTool::handleWheelEvent(QWheelEvent * event){
    Q_UNUSED(event)
}

void GroupTool::handleEnterEvent(QEvent * event){
    Q_UNUSED(event)
}

void GroupTool::handleLeaveEvent(QEvent * event){
    Q_UNUSED(event)
}

void GroupTool::handleKeyPressEvent(QKeyEvent *event){
    Q_UNUSED(event)
}

void GroupTool::handleKeyReleaseEvent(QKeyEvent *event){
    Q_UNUSED(event)
}
