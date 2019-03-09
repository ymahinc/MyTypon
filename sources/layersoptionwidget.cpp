#include "layersoptionwidget.h"
#include "ui_layersoptionwidget.h"
#include "global.h"
#include <QDebug>
#include <QColorDialog>

LayersOptionWidget::LayersOptionWidget(QWidget *parent)
    : QWidget(parent),ui(new Ui::LayersOptionWidget){
    ui->setupUi(this);

    for (int i=0; i < ui->tableWidget->rowCount(); i++ )
        ui->tableWidget->setRowHeight(i,16);

    ui->tableWidget->setColumnWidth(0,200);
    ui->tableWidget->setColumnWidth(1,20);

    drillColor = qApp->workSpace()->drillColor;
    TSColor = qApp->workSpace()->TSColor;
    TCColor = qApp->workSpace()->TCColor;
    BCColor = qApp->workSpace()->BCColor;
    BSColor = qApp->workSpace()->BSColor;
    viaColor = qApp->workSpace()->viaColor;
    
    QPixmap pixmap(12,12);
    pixmap.fill(drillColor);
    QIcon colorIcon(pixmap);
    ui->tableWidget->item(0,1)->setIcon(colorIcon);
    
    QPixmap pixmap1(12,12);
    pixmap1.fill(TSColor);
    QIcon colorIcon1(pixmap1);
    ui->tableWidget->item(1,1)->setIcon(colorIcon1);
    
    QPixmap pixmap2(12,12);
    pixmap2.fill(TCColor);
    QIcon colorIcon2(pixmap2);
    ui->tableWidget->item(2,1)->setIcon(colorIcon2);
    
    QPixmap pixmap3(12,12);
    pixmap3.fill(BCColor);
    QIcon colorIcon3(pixmap3);
    ui->tableWidget->item(3,1)->setIcon(colorIcon3);
    
    QPixmap pixmap4(12,12);
    pixmap4.fill(BSColor);
    QIcon colorIcon4(pixmap4);
    ui->tableWidget->item(4,1)->setIcon(colorIcon4);
    
    QPixmap pixmap5(12,12);
    pixmap5.fill(viaColor);
    QIcon colorIcon5(pixmap5);
    ui->tableWidget->item(5,1)->setIcon(colorIcon5);
    
    connect(ui->tableWidget,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(onDoubleClick(int,int)));
}

LayersOptionWidget::~LayersOptionWidget(){
    delete ui;
}

void LayersOptionWidget::onDoubleClick(int row, int column){

    if ( !ui->tableWidget->item(row,column) )
        return;

    QColor oldColor;
    switch(row){
    case 0:
        oldColor = drillColor;
        break;
    case 1:
        oldColor = TSColor;
        break;
    case 2:
        oldColor = TCColor;
        break;
    case 3:
        oldColor = BCColor;
        break;
    case 4:
        oldColor = BSColor;
        break;
    case 5:
        oldColor = viaColor;
        break;
    }

    QColorDialog colorDLG(oldColor,this);
    if ( colorDLG.exec() == QDialog::Accepted ){
        QColor newColor = colorDLG.currentColor();

        switch(row){
        case 0:
            drillColor = newColor;
            break;
        case 1:
            TSColor = newColor;
            break;
        case 2:
            TCColor = newColor;
            break;
        case 3:
            BCColor = newColor;
            break;
        case 4:
            BSColor = newColor;
            break;
        case 5:
            viaColor = newColor;
            break;
        }

        QPixmap pixmap(12,12);
        pixmap.fill(newColor);
        QIcon colorIcon(pixmap);
        ui->tableWidget->item(row,1)->setIcon(colorIcon);
    }
}
