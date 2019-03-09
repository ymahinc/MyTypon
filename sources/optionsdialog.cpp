#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include <QSettings>
#include <QDebug>

OptionsDialog::OptionsDialog(MainWindow *parent, QString page) :
    QDialog(parent), ui(new Ui::OptionsDialog), m_parent(parent){
    ui->setupUi(this);

    // fill current typon options with infos
    ui->currentTyponOptionsPage->setTyponSize(qApp->currentTypon()->typonSize());
    ui->currentTyponOptionsPage->setCopperThickness(qApp->currentTypon()->copperThickness());

    //hide column that represent the index of the stackedWidget to show
    ui->treeWidget->setColumnHidden(1,true);    

    //select widget corresponding to the page parameter
    QTreeWidgetItem *it = ui->treeWidget->findItems(page,Qt::MatchExactly,1).at(0);
    if ( it ){
        ui->treeWidget->setItemSelected(it, true);
        ui->stackedWidget->setCurrentIndex(it->text(1).toInt());
    }    

    connect(ui->treeWidget,SIGNAL(itemSelectionChanged()),this,SLOT(changeOptionPage()));
}

OptionsDialog::~OptionsDialog(){
    delete ui;
}

// save all on quit if ok button was clicked
void OptionsDialog::on_buttonBox_accepted(){
    ui->generalOptionsPage->saveWorkSpace();
    ui->generalOptionsPage->saveOptions();
    qApp->currentTypon()->setTyponSize(ui->currentTyponOptionsPage->typonSize());
    qApp->currentTypon()->setCopperThickness(ui->currentTyponOptionsPage->copperThickness());
    ui->gridOptionsPage->saveGridSettings();
    ui->librariesPage->saveWorkSpace();
    qApp->workSpace()->drillColor = ui->layersPage->drillColor;
    qApp->workSpace()->TSColor = ui->layersPage->TSColor;
    qApp->workSpace()->TCColor = ui->layersPage->TCColor;
    qApp->workSpace()->BCColor = ui->layersPage->BCColor;
    qApp->workSpace()->BSColor = ui->layersPage->BSColor;
    qApp->workSpace()->viaColor = ui->layersPage->viaColor;
    emit qApp->workSpace()->workspaceUpdated();
}

void OptionsDialog::changeOptionPage(){
    ui->stackedWidget->setCurrentIndex(ui->treeWidget->currentItem()->text(1).toInt());
}
