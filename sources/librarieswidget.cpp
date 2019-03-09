#include "librarieswidget.h"
#include "ui_librarieswidget.h"
#include "global.h"
#include <QFileDialog>
#include <QDebug>

LibrariesWidget::LibrariesWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::LibrariesWidget){
    ui->setupUi(this);

    ui->listWidget->addItems(qApp->workSpace()->librariesPaths);

    if ( ui->listWidget->count() )
        ui->listWidget->setCurrentItem(ui->listWidget->item(0));

    updateButtons();

    connect(ui->addPushButton,SIGNAL(clicked(bool)),this,SLOT(addLibrary()));
    connect(ui->removePushButton,SIGNAL(clicked(bool)),this,SLOT(removeLibrary()));
    connect(ui->downPushButton,SIGNAL(clicked(bool)),this,SLOT(moveDown()));
    connect(ui->upPushButton,SIGNAL(clicked(bool)),this,SLOT(moveUp()));
    connect(ui->listWidget,SIGNAL(itemSelectionChanged()),this,SLOT(updateButtons()));
}

LibrariesWidget::~LibrariesWidget(){
    delete ui;
}

void LibrariesWidget::moveUp(){
    QListWidgetItem *current = ui->listWidget->currentItem();
    int currIndex = ui->listWidget->row(current);

    QListWidgetItem *prev = ui->listWidget->item(ui->listWidget->row(current) - 1);
    int prevIndex = ui->listWidget->row(prev);

    QListWidgetItem *temp = ui->listWidget->takeItem(prevIndex);
    ui->listWidget->insertItem(prevIndex, current);
    ui->listWidget->insertItem(currIndex, temp);

    updateButtons();
}

void LibrariesWidget::moveDown(){
    QListWidgetItem *current = ui->listWidget->currentItem();
    int currIndex = ui->listWidget->row(current);

    QListWidgetItem *next = ui->listWidget->item(ui->listWidget->row(current) + 1);
    int nextIndex = ui->listWidget->row(next);

    QListWidgetItem *temp = ui->listWidget->takeItem(nextIndex);
    ui->listWidget->insertItem(currIndex, temp);
    ui->listWidget->insertItem(nextIndex, current);

    updateButtons();
}

void LibrariesWidget::updateButtons(){
    ui->downPushButton->setEnabled(true);
    ui->upPushButton->setEnabled(true);
    ui->removePushButton->setEnabled(true);

    if ( ui->listWidget->selectedItems().count() == 0 ){
        ui->downPushButton->setEnabled(false);
        ui->upPushButton->setEnabled(false);
        ui->removePushButton->setEnabled(false);
    }else{
        if ( ui->listWidget->currentItem() == ui->listWidget->item(0) )
            ui->upPushButton->setEnabled(false);
        if ( ui->listWidget->currentItem() == ui->listWidget->item(ui->listWidget->count()-1) )
            ui->downPushButton->setEnabled(false);
    }
}

void LibrariesWidget::saveWorkSpace(){
   qApp->workSpace()->librariesPaths = librariesPaths();
}

QStringList LibrariesWidget::librariesPaths(){
    QStringList pathsList;
    for ( int i = 0; i < ui->listWidget->count(); i++)
        pathsList << ui->listWidget->item(i)->text();
    return pathsList;
}

void LibrariesWidget::addLibrary(){
    QString path = QFileDialog::getExistingDirectory(this, tr("Select new library path"),QDir::homePath(),
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);
    if ( ! path.isEmpty() ){
        ui->listWidget->addItem(path);
        ui->listWidget->item(0)->setSelected(true);

        if ( ui->listWidget->count() )
            ui->listWidget->setCurrentItem(ui->listWidget->item(0));

        updateButtons();
    }
}

void LibrariesWidget::removeLibrary(){
    if ( ui->listWidget->selectedItems().count() )
        delete ui->listWidget->selectedItems().at(0);
    if ( ui->listWidget->count() )
        ui->listWidget->setCurrentItem(ui->listWidget->item(0));
    updateButtons();
}
