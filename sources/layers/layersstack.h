#ifndef LAYERSSTACK_H
#define LAYERSSTACK_H

#include <QObject>
#include "layer.h"
#include <QMap>
#include <QVector>

class LayersStack : public QObject
{
    Q_OBJECT

public:
    LayersStack(QGraphicsScene *parent = Q_NULLPTR);
    int count();
    Layer *at(int index);
    Layer *activeLayer();
    int indexOf(Layer *layer);
    void editLayer(Layer *layer, QString name, QColor color, bool visible = true, bool enable = true);
    void setActiveLayer(Layer *layer);
    void addLayer(Layer *layer);
    void insertLayer(Layer *layer, int index = -1);
    void removeLayer(Layer* layer);
    bool activeLayerIsFirstCustomLayer();
    bool activeLayerIsLastCustomLayer();
    void moveLayerDown(Layer *layer);
    void moveLayerUp(Layer *layer);
    Layer *drillLayer();
    Layer *viaLayer();
    bool contains(Layer *layer);
    QVector<Layer *> customLayers();
    Layer *layerFromZValue(int zValue);

signals:
    void layersStackUpdated();
    void layerUpdated(Layer *layer);
    void activeLayerChanged();

private slots:
    void updateColors();

private:
   void generateSystemLayers();

   Layer* m_activeLayer;
   QVector<Layer *> m_layerVector;
   Layer *m_drillLayer;
   Layer *m_viaLayer;
   QGraphicsScene *m_parent;
};

#endif // LAYERSSTACK_H
