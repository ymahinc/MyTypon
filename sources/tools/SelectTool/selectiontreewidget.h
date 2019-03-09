#ifndef SELECTIONTREEWIDGET_H
#define SELECTIONTREEWIDGET_H

/*
 * custom tree widget that display selected items and provide a way to modify their properties
 */

#include <QTreeWidget>
#include "items/myitembase.h"
#include "typonitemtreeitem.h"

class SelectionTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    SelectionTreeWidget(QWidget *parent = Q_NULLPTR);
    void updateList();

public slots:
    void updateName(MYItemBase* item);

private slots:
    void expand(QTreeWidgetItem *item);
    void onCustomContextMenuRequested(const QPoint &pos);
    void removeSelected();

private:
    QList<MYItemBase *> m_selectedItems;
    TyponItemTreeItem *getTreeItemFromTyponItem(MYItemBase *item);
};

#endif // SELECTIONTREEWIDGET_H
