#include "layersstack.h"
#include <QDebug>
#include "items/myitembase.h"
#include "global.h"
#include <QVector>
#include <QMessageBox>
#include "items/paditem/paditem.h"

LayersStack::LayersStack(QGraphicsScene *parent)
    : QObject(parent), m_parent(parent) {
    generateSystemLayers();
    connect(qApp->workSpace(),SIGNAL(workspaceUpdated()),this,SLOT(updateColors()));
}

Layer *LayersStack::layerFromZValue(int zValue){
    if ( (zValue < 0) || (zValue > (m_layerVector.count() -1)) )
        return 0;
    if ( zValue == 0 )
        return m_viaLayer;
    if ( zValue == 65535 )
        return m_drillLayer;
    return m_layerVector.at(zValue);
}

void LayersStack::updateColors(){
    m_viaLayer->setColor(qApp->workSpace()->viaColor);
    m_layerVector.at(1)->setColor(qApp->workSpace()->BSColor);
    m_layerVector.at(2)->setColor(qApp->workSpace()->BCColor);
    m_layerVector.at(m_layerVector.count()-3)->setColor(qApp->workSpace()->TCColor);
    m_layerVector.at(m_layerVector.count()-2)->setColor(qApp->workSpace()->TSColor);
    m_drillLayer->setColor(qApp->workSpace()->drillColor);
    emit layersStackUpdated();
}

QVector<Layer *> LayersStack::customLayers(){
    QVector<Layer *> customVector = m_layerVector;
    customVector.removeFirst();
    customVector.removeFirst();
    customVector.removeFirst();
    customVector.removeLast();
    customVector.removeLast();
    customVector.removeLast();
    return customVector;
}

void LayersStack::generateSystemLayers(){
    m_drillLayer = new Layer(0,tr("Drill"),qApp->workSpace()->drillColor);
    m_drillLayer->setSystem(true);
    Layer *topSerigraphy = new Layer(0,tr("Top serigraphy"),qApp->workSpace()->TSColor);
    topSerigraphy->setSystem(true);
    Layer *topCopper = new Layer(0,tr("Top"),qApp->workSpace()->TCColor);
    topCopper->setSystem(true);
    Layer *bottomCopper = new Layer(0,tr("Bottom"),qApp->workSpace()->BCColor);
    bottomCopper->setSystem(true);
    Layer *bottomSerigraphy = new Layer(0,tr("Bottom serigraphy"),qApp->workSpace()->BSColor);
    bottomSerigraphy->setSystem(true);
    m_viaLayer = new Layer(0,tr("Via"),qApp->workSpace()->viaColor); // unused only for corresponding index in qvector with zvalue
    m_viaLayer->setSystem(true);
    m_layerVector.prepend(m_drillLayer);
    m_parent->addItem(m_drillLayer);
    m_layerVector.prepend(topSerigraphy);
    m_parent->addItem(topSerigraphy);
    m_layerVector.prepend(topCopper);
    m_parent->addItem(topCopper);
    m_layerVector.prepend(bottomCopper);
    m_parent->addItem(bottomCopper);
    m_layerVector.prepend(bottomSerigraphy);
    m_parent->addItem(bottomSerigraphy);
    m_layerVector.prepend(m_viaLayer);
    m_parent->addItem(m_viaLayer);
    m_activeLayer = bottomCopper;
    m_drillLayer->setZValue(65535);
    m_viaLayer->setZValue(65533);
    m_activeLayer->setZValue(65532);
    emit layersStackUpdated();
}

bool LayersStack::contains(Layer *layer){
    bool containLayer = false;
    for(int i =0; i< m_layerVector.count(); i++){
        if ( m_layerVector.at(i) == layer ){
            containLayer = true;
            break;
        }
    }
    return containLayer;
}

Layer *LayersStack::drillLayer(){
    return m_drillLayer;
}

Layer *LayersStack::viaLayer(){
    return m_viaLayer;
}

int LayersStack::count(){
    return m_layerVector.count();
}

Layer *LayersStack::at(int index){
    return m_layerVector.at(index);
}

int LayersStack::indexOf(Layer *layer){
    return m_layerVector.indexOf(layer);
}

// update layer with new values
void LayersStack::editLayer(Layer *layer, QString name, QColor color, bool visible, bool enable){
    layer->setName(name);
    layer->setColor(color);
    // if visibility or enable has chaned, clear selection for layer
    if ( visible != layer->isVisible() || enable != layer->isEnabled() )
        layer->clearSelection();
    layer->setVisible(visible);
    layer->setEnabled(enable);
    qApp->currentTypon()->typonScene()->invalidate();
    emit layerUpdated(layer);
}

void LayersStack::addLayer(Layer *layer){
    if ( m_layerVector.count() == 65536 ){
        QMessageBox::warning(0, tr("Layers"), tr("You have reached the maximum number of layers you can add!"));
        return;
    }    
    m_layerVector.insert(m_layerVector.count()-3,layer);
    layer->updateTransparency();
    layer->updateZValue();
    setActiveLayer(layer);
    m_parent->addItem(layer);
    emit layersStackUpdated();
}

Layer *LayersStack::activeLayer(){
    return m_activeLayer;
}

// update active layer
void LayersStack::setActiveLayer(Layer *layer){
    Layer *oldLayer = m_activeLayer;
    m_activeLayer = layer;
    oldLayer->updateZValue();
    m_activeLayer->updateZValue();
    qApp->currentTypon()->typonScene()->invalidate();

    emit activeLayerChanged();
}

void LayersStack::insertLayer(Layer *layer, int index){
    m_layerVector.insert(index,layer);
    layer->updateTransparency();
    layer->updateZValue();
    setActiveLayer(layer);
    m_parent->addItem(layer);
    emit layersStackUpdated();
}

void LayersStack::removeLayer(Layer* layer){
    // if layer to delete is the current active one, set bottom copper layer as active
    if ( layer == m_activeLayer )
        setActiveLayer(m_layerVector.at(2));
    m_layerVector.removeOne(layer);
    m_parent->removeItem(layer);
    emit layersStackUpdated();
}

// check wether the current layer is the first system one
// used not to move down a custom layer below system ones
bool LayersStack::activeLayerIsFirstCustomLayer(){
    if ( m_activeLayer->isSystem() )
        return false;

    int i = m_layerVector.indexOf(m_activeLayer);
    return i == 3;
}

// check wether the current layer is the last system one
// used not to move down a custom layer uppon system ones
bool LayersStack::activeLayerIsLastCustomLayer(){
    if ( m_activeLayer->isSystem() )
        return false;

    int i = m_layerVector.indexOf(m_activeLayer);
    return i == m_layerVector.count() - 4;
}

//swap layers up
void LayersStack::moveLayerUp(Layer *layer){
    int i = indexOf(layer);
    Layer* temp = m_layerVector.at(i+1);
    m_layerVector[i+1] = layer;
    m_layerVector[i] = temp;
    layer->updateZValue();
    temp->updateZValue();
    emit layersStackUpdated();
}

//swap layers down
void LayersStack::moveLayerDown(Layer *layer){
    int i = indexOf(layer);
    Layer* temp = m_layerVector.at(i-1);
    m_layerVector[i-1] = layer;
    m_layerVector[i] = temp;
    layer->updateZValue();
    temp->updateZValue();
    emit layersStackUpdated();
}
