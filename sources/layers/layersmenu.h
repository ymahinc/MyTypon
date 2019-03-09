#ifndef LAYERSMENU_H
#define LAYERSMENU_H

#include <QMenu>
#include <QObject>
#include "layer.h"
#include <QActionGroup>

class LayersMenu : public QMenu
{
    Q_OBJECT

public:
    LayersMenu(QWidget *parent = Q_NULLPTR);

signals:
    void sendToLayer(Layer *layer);

private slots:
    void updateMenu();
    void onActionTriggered(QAction *action);

private:
    QActionGroup *m_actionGroup;
};

#endif // LAYERSMENU_H
