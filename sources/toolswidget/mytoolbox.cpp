#include "mytoolbox.h"
#include "ui_mytoolbox.h"
#include <QPushButton>

MYToolBox::MYToolBox(QWidget *parent)
    : QWidget(parent), ui(new Ui::MYToolBox){
    ui->setupUi(this);
    ui->scrollArea->setWidgetResizable(true);

    m_buttonGroup = new QButtonGroup(this); // group of buttons representing tools
    m_flowLayout = new FlowLayout(ui->flowWidget);
    m_flowLayout->setSpacing(0);
    m_flowLayout->setMargin(0);
    ui->flowWidget->setLayout(m_flowLayout);

    connect(m_buttonGroup,SIGNAL(buttonClicked(int)),this,SLOT(setCurrentIndex(int)));
    connect(m_buttonGroup,SIGNAL(buttonClicked(int)),this,SIGNAL(currentChanged(int)));
}

MYToolBox::~MYToolBox(){
    delete ui;
}


// add a tool: add a button to the button group and a stackedwidget page with the widget for the tool's options
void MYToolBox::addTool(MYToolBase *tool){
    QPushButton *button = new QPushButton(this);
    button->setCheckable(true);
    button->setFlat(true);
    button->setIcon(tool->toolIcon());
    button->setToolTip(tool->toolName());
    m_buttonGroup->addButton(button,ui->stackedWidget->count());
    m_flowLayout->addWidget(button);
    ui->stackedWidget->addWidget(tool);
}

void MYToolBox::setCurrentIndex(int index){
    ui->stackedWidget->setCurrentIndex(index);
    m_buttonGroup->button(index)->setChecked(true);
    emit currentChanged(index);
}

QWidget *MYToolBox::widget(int index){
    return ui->stackedWidget->widget(index);
}
