#include "typonitemtreeitem.h"

TyponItemTreeItem::TyponItemTreeItem(QTreeWidget *parent, int type, MYItemBase *item)
    :   QTreeWidgetItem(parent, type), m_item(item) {
    setChildIndicatorPolicy(ShowIndicator);
    setText(0,item->itemName());
    setBackgroundColor(0,QColor(240,240,240));
    setBackgroundColor(1,QColor(240,240,240));
}

MYItemBase *TyponItemTreeItem::item(){
    return m_item;
}
