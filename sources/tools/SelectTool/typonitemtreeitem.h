#ifndef TYPONITEMTREEITEM_H
#define TYPONITEMTREEITEM_H

/*
 * Custom treeWidgetItem that keeps a pointer to an MYItemBase
 */

#include <QTreeWidgetItem>
#include "items/myitembase.h"

class TyponItemTreeItem : public QTreeWidgetItem
{

public:
    TyponItemTreeItem(QTreeWidget *parent, int type = 1001, MYItemBase *item = 0);
    MYItemBase *item();

private:
    MYItemBase *m_item;
};

#endif // TYPONITEMTREEITEM_H
