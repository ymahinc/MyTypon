#ifndef LAYERSWIDGET_H
#define LAYERSWIDGET_H

// principal widget of the layers dock
// display, and edit a layer stack
// layers are displayed inreversed order of layerstack vector

#include <QWidget>
#include <QObject>
#include "../layers/layer.h"
#include "../layers/layersstack.h"
#include <QMap>

namespace Ui {
class LayersWidget;
}

class LayersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LayersWidget(QWidget *parent = 0);
    ~LayersWidget();
    void setLayersStack(LayersStack * stack);    

private slots:
    void cellDoubleClicked(int row, int column);
    void updateToolButtons();
    void onLayersStackUpdated();
    void on_modifyLayerPushButton_clicked();
    void on_addLayerPushButton_clicked();
    void on_deleteLayerPushButton_clicked();
    void on_layerDownPushButton_clicked();
    void on_layerUpPushButton_clicked();
    void updateActiveLayer();
    void layerUpdated(Layer *layer);

private:
    void addLayerRow(Layer *layer);

    Ui::LayersWidget *ui;
    LayersStack *m_layersStack;
};

#endif // LAYERSWIDGET_H
