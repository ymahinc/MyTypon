#include "gridwidget.h"
#include "ui_gridwidget.h"
#include "global.h"

// when created, load settings anf update ui according to global workspace
GridWidget::GridWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::GridWidget){
    ui->setupUi(this);

    ui->snapToGridCheckBox->setChecked(qApp->workSpace()->snapToGrid);
    setFixedSize(this->width(),this->height());
    ui->showGridCheckBox->setChecked(qApp->workSpace()->grid->enabled());
    ui->styleComboBox->setCurrentIndex(qApp->workSpace()->grid->gridType());

    ui->xStepSpinBox->setRawValue(qApp->workSpace()->grid->xStepGrid());
    ui->yStepSpinBox->setRawValue(qApp->workSpace()->grid->yStepGrid());

    ui->gridColorPushButton->setColor(qApp->workSpace()->grid->gridColor());
    ui->bgColorPushButton->setColor(qApp->workSpace()->grid->backgroundColor());

    ui->dynamicGridCheckBox->setChecked(qApp->workSpace()->grid->dynamicGrid());

    ui->mainLinesSpinBox->setValue(qApp->workSpace()->grid->mainLinesSubDivs());

    // activate or deactivate mainline spinbox according to mainline option
    connect(ui->mainLinesSpinBox,SIGNAL(valueChanged(int)),qApp->workSpace()->grid,SLOT(setMainLinesSubDiv(int)));
    connect(ui->mainLinesCheckBox,SIGNAL(stateChanged(int)),this,SLOT(onMainLinesCheckChanged(int)));

    ui->mainLinesCheckBox->setChecked(qApp->workSpace()->grid->mainLines());
}

GridWidget::~GridWidget(){
    delete ui;
}

void GridWidget::onMainLinesCheckChanged(int checked){
    ui->mainLinesSpinBox->setEnabled(checked);
}

// save grid settings in current workspace when options dialog is closed
void GridWidget::saveGridSettings(){
    qApp->workSpace()->grid->setEnabled(ui->showGridCheckBox->isChecked());
    qApp->workSpace()->grid->setGridType((MYTyponGrid::GridTypes)ui->styleComboBox->currentIndex());
    qApp->workSpace()->grid->setXStepGrid(ui->xStepSpinBox->rawValue());
    qApp->workSpace()->grid->setYStepGrid(ui->yStepSpinBox->rawValue());
    qApp->workSpace()->grid->setGridColor(ui->gridColorPushButton->color());
    qApp->workSpace()->grid->setBackgroundColor(ui->bgColorPushButton->color());
    qApp->workSpace()->grid->setMainLines(ui->mainLinesCheckBox->isChecked());
    qApp->workSpace()->grid->setDynamicGrid(ui->dynamicGridCheckBox->isChecked());
    qApp->workSpace()->grid->setMainLinesSubDiv(ui->mainLinesSpinBox->value());
    qApp->workSpace()->snapToGrid = ui->snapToGridCheckBox->isChecked();
}
