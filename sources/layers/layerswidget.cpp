#include "layerswidget.h"
#include "ui_layerswidget.h"
#include "global.h"
#include <QDebug>
#include "layeroptionsdialog.h"
#include <QMessageBox>
#include "addlayerundocommand.h"
#include "removelayerundocommand.h"
#include "editlayerundocommand.h"
#include "movelayerundocommand.h"
#include "items/myitembase.h"
#include "layerstableitem.h"

LayersWidget::LayersWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::LayersWidget) {
    ui->setupUi(this);

    m_layersStack = 0;
    ui->layersTable->setColumnWidth(0,20);
    ui->layersTable->setColumnWidth(1,20);
    ui->layersTable->setColumnWidth(2,20);
    ui->layersTable->setStyleSheet("QTableView {selection-background-color: rgba(127,127,255,255);}");

    connect(ui->layersTable,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(cellDoubleClicked(int,int)));
    connect(ui->layersTable,SIGNAL(itemSelectionChanged()),this,SLOT(updateActiveLayer()));
}

LayersWidget::~LayersWidget(){
    delete ui;
}

void LayersWidget::setLayersStack(LayersStack * stack){
    // set the new stack for the widget
    m_layersStack = stack;
    connect(m_layersStack,SIGNAL(layersStackUpdated()),this,SLOT(onLayersStackUpdated()));
    connect(m_layersStack,SIGNAL(layerUpdated(Layer*)),this,SLOT(layerUpdated(Layer*)));
    // load the stack
    onLayersStackUpdated();
}

// update layer raw without reload all tableWidget
void LayersWidget::layerUpdated(Layer *layer){
    for (int i = 0; i < ui->layersTable->rowCount(); i++ ){
        LayersTableItem *item = dynamic_cast<LayersTableItem *>(ui->layersTable->item(i,0));
        if ( item ){
            if ( item->layer() == layer ){
                if ( layer->isVisible() )
                    ui->layersTable->item(i,0)->setIcon(QIcon(":/pictures/eye.png"));
                else
                    ui->layersTable->item(i,0)->setIcon(QIcon(":/pictures/eyenot.png"));

                if ( layer->isSystem() ){
                    ui->layersTable->item(i,0)->setBackgroundColor(Qt::lightGray);
                    ui->layersTable->item(i,1)->setBackgroundColor(Qt::lightGray);
                    ui->layersTable->item(i,2)->setBackgroundColor(Qt::lightGray);
                    ui->layersTable->item(i,3)->setBackgroundColor(Qt::lightGray);
                }

                if ( layer->isEnabled() )
                    ui->layersTable->item(i,1)->setIcon(QIcon());
                else
                    ui->layersTable->item(i,1)->setIcon(QIcon(":/pictures/lock.png"));

                QPixmap pixmap(12,12);
                pixmap.fill(layer->color());
                QIcon colorIcon(pixmap);
                ui->layersTable->item(i,2)->setIcon(colorIcon);

                ui->layersTable->item(i,3)->setText(layer->name());

                break;
            }
        }
    }
}

// select new active layer if new row was selected
void LayersWidget::updateActiveLayer(){
    if ( m_layersStack == 0 )
        return;
    if ( ! ui->layersTable->selectedItems().count() )
        return;
    LayersTableItem *item = dynamic_cast<LayersTableItem *>(ui->layersTable->selectedItems().at(0));
    if ( item ){
        m_layersStack->setActiveLayer(item->layer());
        updateToolButtons();
    }
}

// if layers stack has changed update widget: clear all and reload
// could be optimized
void LayersWidget::onLayersStackUpdated(){
    if ( m_layersStack == 0 )
        return;
    disconnect(ui->layersTable,SIGNAL(itemSelectionChanged()),this,SLOT(updateToolButtons()));
    ui->layersTable->setRowCount(0);
    for ( int i = m_layersStack->count() - 1; i > 0  ;i-- ){
        addLayerRow(m_layersStack->at(i));
    }
    updateToolButtons();
    connect(ui->layersTable,SIGNAL(itemSelectionChanged()),this,SLOT(updateToolButtons()));
}

// add a row wich represent a layer in the widget
void LayersWidget::addLayerRow(Layer *layer){
    int row = ui->layersTable->rowCount();
    ui->layersTable->insertRow(row);
    ui->layersTable->setRowHeight(row,16);

    // if layer to add is the active one, select it
    if ( layer == m_layersStack->activeLayer() )
       ui->layersTable->selectRow(row);

    LayersTableItem *itemVisible = new LayersTableItem(layer, 1001);
    if ( layer->isVisible() )
        itemVisible->setIcon(QIcon(":/pictures/eye.png"));
    else
        itemVisible->setIcon(QIcon(":/pictures/eyenot.png"));
    if ( layer->isSystem() )
        itemVisible->setBackgroundColor(Qt::lightGray);
    ui->layersTable->setItem(row, 0, itemVisible);

    LayersTableItem *itemLocked = new LayersTableItem(layer, 1001);
    if ( layer->isEnabled() )
        itemLocked->setIcon(QIcon());
    else
        itemLocked->setIcon(QIcon(":/pictures/lock.png"));
    if ( layer->isSystem() )
        itemLocked->setBackgroundColor(Qt::lightGray);
    ui->layersTable->setItem(row, 1, itemLocked);

    LayersTableItem *itemColor = new LayersTableItem(layer, 1001);
    QPixmap pixmap(12,12);
    pixmap.fill(layer->color());
    QIcon colorIcon(pixmap);
    itemColor->setIcon(colorIcon);
    if ( layer->isSystem() )
        itemColor->setBackgroundColor(Qt::lightGray);
    ui->layersTable->setItem(row, 2, itemColor);

    LayersTableItem *itemName = new LayersTableItem(layer, 1001);
    itemName->setText(layer->name());
    if ( layer->isSystem() )
        itemName->setBackgroundColor(Qt::lightGray);
    ui->layersTable->setItem(row, 3, itemName);
}

// change the visibility or lock icon, show the layer dialogs in order to modify current layer
// depending what item is clicked
void LayersWidget::cellDoubleClicked(int row, int column){
    LayersTableItem *item = dynamic_cast<LayersTableItem *>(ui->layersTable->item(row,column));
    if ( !item )
        return;

    if ( item->layer()->isSystem() )
        return;

    if ( column == 2 || column == 3 ){ // click on name or color
        LayerOptionsDialog *dialog = new LayerOptionsDialog(this);
        dialog->setLayer(item->layer());
        // show the edit layer dialog, modify color and name if ok button is clicked
        if ( dialog->exec() == QDialog::Accepted ){
            EditLayerUndoCommand *editLayerCommand = new EditLayerUndoCommand(m_layersStack,item->layer(),dialog->getColor(),dialog->getName());
            qApp->undoGroup()->activeStack()->push(editLayerCommand);
        }
    }
    if ( column == 0 ){ // clic on visibility
        bool visible = !item->layer()->isVisible();
        m_layersStack->editLayer(item->layer(),item->layer()->name(),item->layer()->color(),visible, item->layer()->isEnabled());
    }
    if ( column == 1 ){ // clic on lock
        bool enable = !item->layer()->isEnabled();
        m_layersStack->editLayer(item->layer(),item->layer()->name(),item->layer()->color(),item->layer()->isVisible(), enable);
    }
}

// update availability of tool button, depending if current layer is a system one
void LayersWidget::updateToolButtons(){
    // if clic with control key pressed, could have no more selected row
    if ( ! ui->layersTable->selectedItems().count() ){
        // loop the tablewidget until finding the current active layer and select it
        for (int i = 0; i < ui->layersTable->rowCount(); i++ ){
            LayersTableItem *item = dynamic_cast<LayersTableItem *>(ui->layersTable->item(i,0));
            if ( item ){
                if ( item->layer() == m_layersStack->activeLayer() ){
                    disconnect(ui->layersTable,SIGNAL(itemSelectionChanged()),this,SLOT(updateToolButtons()));
                    ui->layersTable->selectRow(i);
                    connect(ui->layersTable,SIGNAL(itemSelectionChanged()),this,SLOT(updateToolButtons()));
                }
            }
        }
    }

    // enable or disable edit buttons depending of current active layer
    ui->deleteLayerPushButton->setEnabled(! m_layersStack->activeLayer()->isSystem());
    ui->modifyLayerPushButton->setEnabled(! m_layersStack->activeLayer()->isSystem());
    if (  m_layersStack->activeLayer()->isSystem() ){
        ui->layerDownPushButton->setEnabled(false);
        ui->layerUpPushButton->setEnabled(false);
    }else{
        ui->layerDownPushButton->setEnabled(! m_layersStack->activeLayerIsFirstCustomLayer());
        ui->layerUpPushButton->setEnabled(! m_layersStack->activeLayerIsLastCustomLayer());
    }
}

void LayersWidget::on_modifyLayerPushButton_clicked(){
    cellDoubleClicked(ui->layersTable->selectedItems().at(0)->row() , 2);
}

// add new layer
void LayersWidget::on_addLayerPushButton_clicked(){
    LayerOptionsDialog *dialog = new LayerOptionsDialog(this);
    if ( dialog->exec() == QDialog::Accepted ){
        Layer *layer = new Layer(0,dialog->getName(),dialog->getColor());
        AddLayerUndoCommand *addLayerCommand = new AddLayerUndoCommand(m_layersStack,layer);
        qApp->undoGroup()->activeStack()->push(addLayerCommand);
    }
}

//delete selected layer
void LayersWidget::on_deleteLayerPushButton_clicked(){
    LayersTableItem *item = dynamic_cast<LayersTableItem *>(ui->layersTable->selectedItems().at(0));
    if ( item ){
        RemoveLayerUndoCommand *removeLayerCommand = new RemoveLayerUndoCommand(m_layersStack,item->layer(),item->row());
        qApp->undoGroup()->activeStack()->push(removeLayerCommand);
    }
}

// move selected layer down
void LayersWidget::on_layerDownPushButton_clicked(){
    MoveLayerUndoCommand *moveLayerCommand = new MoveLayerUndoCommand(m_layersStack,m_layersStack->activeLayer(),false);
    qApp->undoGroup()->activeStack()->push(moveLayerCommand);
}

// move selected layer up
void LayersWidget::on_layerUpPushButton_clicked(){
    MoveLayerUndoCommand *moveLayerCommand = new MoveLayerUndoCommand(m_layersStack,m_layersStack->activeLayer(),true);
    qApp->undoGroup()->activeStack()->push(moveLayerCommand);
}
