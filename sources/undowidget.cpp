#include "undowidget.h"
#include "ui_undowidget.h"

UndoWidget::UndoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UndoWidget){
    ui->setupUi(this);
    ui->undoView->setStyleSheet("QListView::item { height: 16px; }");
}

UndoWidget::~UndoWidget(){
    delete ui;
}

QUndoView *UndoWidget::undoView(){
    return ui->undoView;
}
