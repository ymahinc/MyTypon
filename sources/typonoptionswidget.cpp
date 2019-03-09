#include "typonoptionswidget.h"
#include "ui_typonoptionswidget.h"
#include <QDebug>

TyponOptionsWidget::TyponOptionsWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::TyponOptionsWidget){
    ui->setupUi(this);
}

TyponOptionsWidget::~TyponOptionsWidget(){
    delete ui;
}

void TyponOptionsWidget::setTyponSize(QSizeF size){
    ui->typonWidthSpinBox->setRawValue(size.width());
    ui->typonHeightSpinBox->setRawValue(size.height());
}

QSizeF TyponOptionsWidget::typonSize(){
    return QSizeF(ui->typonWidthSpinBox->rawValue(),ui->typonHeightSpinBox->rawValue());
}

void TyponOptionsWidget::setCopperThickness(double value){
    ui->copperThickNessSpinBox->setValue(value);
}

double TyponOptionsWidget::copperThickness(){
    return ui->copperThickNessSpinBox->value();
}
