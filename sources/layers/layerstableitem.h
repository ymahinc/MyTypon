#ifndef LAYERSTABLEITEM_H
#define LAYERSTABLEITEM_H

/*
 * Custom table widget item that store a pointer to the layer it is representing
 */

#include <QTableWidgetItem>
#include "layer.h"

class LayersTableItem : public QTableWidgetItem
{
public:
    LayersTableItem(Layer *layer, int type = Type);
    Layer *layer();

private:
    Layer *m_layer;
};

#endif // LAYERSTABLEITEM_H
