#include "selectiontreewidget.h"
#include "global.h"
#include <QDebug>
#include "items/textitem/textitem.h"
#include "items/paditem/paditem.h"
#include "items/trackitem/track.h"
#include "items/drawitem.h"
#include "tools/PadTool/padtool.h"
#include "tools/TextTool/texttool.h"
#include "tools/GroupTool/grouptool.h"
#include "tools/TrackTool/tracktool.h"
#include "tools/DrawTool/drawtool.h"

SelectionTreeWidget::SelectionTreeWidget(QWidget *parent)
    :   QTreeWidget(parent){
    setFrameStyle(0);
    connect(this,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(expand(QTreeWidgetItem*)));
    //context menu to remove one item from the selection
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(const QPoint&)),SLOT(onCustomContextMenuRequested(const QPoint&)));
}

// update tree content when selection change on the scene
void SelectionTreeWidget::updateList(){
    // get a sopy list of selected items
    // we will remove all items present in the new list from this one
    // so will remain only unselected items in it
    QList<MYItemBase *> unselectedItems = m_selectedItems;

    for ( int i =0; i < qApp->currentTypon()->typonScene()->selectedTyponItems()->count(); i++ ){
        MYItemBase *itB = qApp->currentTypon()->typonScene()->selectedTyponItems()->at(i);
        // if the item is not in the current list, add it
        if ( ! m_selectedItems.contains(itB) ){
            TyponItemTreeItem *item = new TyponItemTreeItem(this,1001,itB);
            addTopLevelItem(item);
            m_selectedItems.append(itB);
        }else{
            // the item is already on the list, remove it from unselected list
            unselectedItems.removeOne(itB);
        }
    }

    // remove all remaining items in the unselected list from the selection
    for ( int i =0; i < unselectedItems.count(); i++ ){
        MYItemBase *it = unselectedItems.at(i);
        m_selectedItems.removeOne(it);
        delete getTreeItemFromTyponItem(it);
        qApp->currentTypon()->typonScene()->removeSelectedItem(it,false);
    }
}

// find tree item handling the MYItemBase passed as parameter
TyponItemTreeItem *SelectionTreeWidget::getTreeItemFromTyponItem(MYItemBase *item){
    TyponItemTreeItem *treeItem = 0;
    for ( int i = 0; i < topLevelItemCount(); i++){
        TyponItemTreeItem *it = dynamic_cast<TyponItemTreeItem *>(topLevelItem(i));
        if ( it ) {
            if ( it->item() == item ){
                TyponItemTreeItem *it = dynamic_cast<TyponItemTreeItem *>(topLevelItem(i));
                if ( it ){
                    if ( it->item() == item ){
                        treeItem = it;
                        break;
                    }
                }
            }
        }
    }
    return treeItem;
}

// if we ask to expand a top level item, show corresponding widget
void SelectionTreeWidget::expand(QTreeWidgetItem *item){
    // if no child, then load a new edit widget for the item
    if ( ! item->childCount() ){
        TyponItemTreeItem *it = dynamic_cast<TyponItemTreeItem *>(item);
        if ( it ) {
            TextItem *textIt = dynamic_cast<TextItem *>(it->item());
            // if item is TextItem, add a TextTool widget
            if ( textIt ) {
                TextTool *textWidget  = new TextTool(this,textIt);
                QTreeWidgetItem *child = new QTreeWidgetItem();
                item->addChild(child);
                setItemWidget(child, 0, textWidget);
                connect(textWidget,SIGNAL(nameChanged(MYItemBase*)),this,SLOT(updateName(MYItemBase*)));
                return;
            }

            Track *trackIt = dynamic_cast<Track *>(it->item());
            // if item is TextItem, add a TextTool widget
            if ( trackIt ) {
                DrawItem *drawIt = dynamic_cast<DrawItem *>(trackIt);
                if ( drawIt ){
                    DrawTool *drawWidget  = new DrawTool(this,drawIt);
                    QTreeWidgetItem *child = new QTreeWidgetItem();
                    item->addChild(child);
                    setItemWidget(child, 0, drawWidget);
                    connect(drawWidget,SIGNAL(nameChanged(MYItemBase*)),this,SLOT(updateName(MYItemBase*)));
                }else{
                    TrackTool *trackWidget  = new TrackTool(this,trackIt);
                    QTreeWidgetItem *child = new QTreeWidgetItem();
                    item->addChild(child);
                    setItemWidget(child, 0, trackWidget);
                    connect(trackWidget,SIGNAL(nameChanged(MYItemBase*)),this,SLOT(updateName(MYItemBase*)));
                }
                return;
            }

            PadGroup *padIt = dynamic_cast<PadGroup *>(it->item());
            // if item is pasItem, add a PadTool widget
            if ( padIt ) {
                PadTool *padWidget  = new PadTool(this,padIt);
                QTreeWidgetItem *child = new QTreeWidgetItem();
                item->addChild(child);
                setItemWidget(child, 0, padWidget);
                connect(padWidget,SIGNAL(nameChanged(MYItemBase*)),this,SLOT(updateName(MYItemBase*)));
                return;
            }

            MYItemBase *groupIt = dynamic_cast<MYItemBase *>(it->item());
            // if item is GroupItem, add a GroupTool widget
            if ( groupIt ) {
                if ( groupIt->isGroup() ) {
                    GroupTool *groupWidget  = new GroupTool(this,groupIt);
                    QTreeWidgetItem *child = new QTreeWidgetItem();
                    item->addChild(child);
                    setItemWidget(child, 0, groupWidget);
                    connect(groupWidget,SIGNAL(nameChanged(MYItemBase*)),this,SLOT(updateName(MYItemBase*)));
                    return;
                }
            }
        }
    }
}

// on right clic on a toplevel item provide a menu to remove it from selection
void SelectionTreeWidget::onCustomContextMenuRequested(const QPoint &pos){
    TyponItemTreeItem *it = dynamic_cast<TyponItemTreeItem *>(itemAt(pos));
    if (it) {
        QMenu menu;
        QAction removeAction;
        removeAction.setText(tr("Remove from selection"));
        menu.addAction(&removeAction);
        connect(&removeAction,SIGNAL(triggered(bool)),this,SLOT(removeSelected()));
        menu.exec(viewport()->mapToGlobal(pos));
    }
}

// remove selected item from selection
void SelectionTreeWidget::removeSelected(){
    TyponItemTreeItem *it = dynamic_cast<TyponItemTreeItem *>(selectedItems().at(0));
    if (it)
        qApp->currentTypon()->typonScene()->removeSelectedItem(it->item());
}

// update toplevel item with new name when modified in an item widget
void SelectionTreeWidget::updateName(MYItemBase* item){
    TyponItemTreeItem *it =getTreeItemFromTyponItem(item);
    if ( it )
        it->setText(0,item->itemName());
}
