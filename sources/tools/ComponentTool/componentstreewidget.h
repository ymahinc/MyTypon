#ifndef COMPONENTSTREEWIDGET_H
#define COMPONENTSTREEWIDGET_H

#include <QTreeWidget>
#include <QFileInfoList>
#include "componentstreewidgetitem.h"

class ComponentsTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    ComponentsTreeWidget(QWidget *parent = Q_NULLPTR);

private slots:
    void onExpand(QTreeWidgetItem *item);
    void reload();
};

#endif // COMPONENTSTREEWIDGET_H
