#include "layerstableitem.h"

LayersTableItem::LayersTableItem(Layer *layer, int type)
    : QTableWidgetItem(type) {
    m_layer = layer;
}

Layer *LayersTableItem::layer(){
    return m_layer;
}
