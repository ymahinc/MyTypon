#ifndef LAYEROPTIONSDIALOG_H
#define LAYEROPTIONSDIALOG_H

#include <QDialog>
#include "layer.h"

namespace Ui {
class LayerOptionsDialog;
}

class LayerOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LayerOptionsDialog(QWidget *parent = 0);
    ~LayerOptionsDialog();
    QString getName();
    QColor getColor();
    void setLayer(Layer *layer);

private:
    Ui::LayerOptionsDialog *ui;
    Layer *m_layer;
};

#endif // LAYEROPTIONSDIALOG_H
