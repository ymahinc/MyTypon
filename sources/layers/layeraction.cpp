#include "layeraction.h"

LayerAction::LayerAction(QObject *parent, Layer *layer)
    :   QAction(parent), m_layer(layer){
    setText(layer->name());
    QPixmap pm(16,16);
    pm.fill(layer->color());
    setIcon(pm);
}

Layer *LayerAction::layer(){
    return m_layer;
}
