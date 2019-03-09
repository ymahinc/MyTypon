#ifndef LAYERACTION_H
#define LAYERACTION_H

#include <QAction>
#include "layer.h"

class LayerAction : public QAction
{
public:
    LayerAction(QObject *parent = nullptr, Layer *layer = 0);
    Layer *layer();

private:
    Layer *m_layer;
};

#endif // LAYERACTION_H
