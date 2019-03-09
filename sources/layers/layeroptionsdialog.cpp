#include "layeroptionsdialog.h"
#include "ui_layeroptionsdialog.h"

LayerOptionsDialog::LayerOptionsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LayerOptionsDialog){
    ui->setupUi(this);
    setWindowTitle(tr("New layer"));
    ui->nameLineEdit->setText("");
    ui->colorButton->setColor(Qt::black);
}

LayerOptionsDialog::~LayerOptionsDialog(){
    delete ui;
}

void LayerOptionsDialog::setLayer(Layer *layer){
    setWindowTitle(QString("%1 %2").arg(layer->name()).arg(tr("layer")));
    ui->nameLineEdit->setText(layer->name());
    ui->colorButton->setColor(layer->color());
}

QString LayerOptionsDialog::getName(){
    return ui->nameLineEdit->text();
}

QColor LayerOptionsDialog::getColor(){
    return ui->colorButton->color();
}
