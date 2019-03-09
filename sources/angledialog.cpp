#include "angledialog.h"
#include "ui_angledialog.h"

AngleDialog::AngleDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AngleDialog){
    ui->setupUi(this);
}

AngleDialog::~AngleDialog(){
    delete ui;
}

double AngleDialog::getAngle(){
    return (double)ui->spinBox->value();
}

