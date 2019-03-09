#include "generaloptionswidget.h"
#include "ui_generaloptionswidget.h"

#include <QDebug>

GeneralOptionsWidget::GeneralOptionsWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::GeneralOptionsWidget) {
    ui->setupUi(this);

    ui->originGroupBox->setVisible(false);
    connect(ui->originTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateOriginPointWidgets(int)));

    // set dialog settings with workspace informations
    ui->unitsComboBox->setCurrentUnit(qApp->workSpace()->unit);
    ui->typonWidthSpinBox->setRawValue(qApp->workSpace()->defaultTyponSize.width());
    ui->typonHeightSpinBox->setRawValue(qApp->workSpace()->defaultTyponSize.height());
    ui->originTypeComboBox->setCurrentIndex(qApp->workSpace()->originType());
    ui->showCurosrLinesCheckBox->setChecked(qApp->workSpace()->linesUnderCursor);
    ui->showRulersCheckBox->setChecked(qApp->workSpace()->showRulers);
    ui->copperThicknessSpinBox->setValue(qApp->workSpace()->copperThickness);
    ui->zoomOnWheelCheckBox->setChecked(qApp->workSpace()->zoomOnWheel);
    ui->saveWorkSpaceAutCheckBox->setChecked(qApp->options()->saveWorkSpaceOnQuit);
    ui->tempRiseSpinBox->setValue(qApp->workSpace()->tempRise);
    ui->trackOpacityCheckBox->setChecked(qApp->workSpace()->semiTransparentTracks);

    ui->xOriginSpinBox->setAllowZero(true);
    ui->yOriginSpinBox->setAllowZero(true);
}

GeneralOptionsWidget::~GeneralOptionsWidget(){
    delete ui;
}

void GeneralOptionsWidget::updateOriginPointWidgets(int index){
    ui->originGroupBox->setVisible(index < 10 ? false : true);
    ui->xOriginSpinBox->setRawValue(qApp->workSpace()->originPos(qApp->workSpace()->defaultTyponSize).x());
    ui->yOriginSpinBox->setRawValue(qApp->workSpace()->originPos(qApp->workSpace()->defaultTyponSize).y());
}

void GeneralOptionsWidget::saveWorkSpace(){
    qApp->workSpace()->linesUnderCursor = ui->showCurosrLinesCheckBox->isChecked();
    qApp->workSpace()->unit = ui->unitsComboBox->currentUnit();
    qApp->workSpace()->showRulers = ui->showRulersCheckBox->isChecked();
    qApp->workSpace()->defaultTyponSize.setHeight(ui->typonHeightSpinBox->rawValue());
    qApp->workSpace()->defaultTyponSize.setWidth(ui->typonWidthSpinBox->rawValue());
    qApp->workSpace()->copperThickness = ui->copperThicknessSpinBox->value();
    qApp->workSpace()->tempRise = ui->tempRiseSpinBox->value();
    qApp->workSpace()->setOriginType((MYWorkSpace::OriginPositionTypes)ui->originTypeComboBox->currentIndex(),
                                     QPointF(ui->xOriginSpinBox->rawValue(),ui->yOriginSpinBox->rawValue()));
    qApp->workSpace()->zoomOnWheel = ui->zoomOnWheelCheckBox->isChecked();
    qApp->workSpace()->semiTransparentTracks = ui->trackOpacityCheckBox->isChecked();
}

void GeneralOptionsWidget::saveOptions(){
    qApp->options()->saveWorkSpaceOnQuit = ui->saveWorkSpaceAutCheckBox->isChecked();
    qApp->options()->save();
}
