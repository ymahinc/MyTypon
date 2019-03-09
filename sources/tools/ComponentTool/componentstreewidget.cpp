#include "componentstreewidget.h"
#include "global.h"
#include <QFileInfo>
#include <QDebug>
#include <QDir>

ComponentsTreeWidget::ComponentsTreeWidget(QWidget *parent)
    : QTreeWidget(parent){
    reload();
    connect(this,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(onExpand(QTreeWidgetItem*)));
    connect(qApp->workSpace(),SIGNAL(workspaceUpdated()),this,SLOT(reload()));
}

void ComponentsTreeWidget::reload(){
    for (int i = topLevelItemCount()-1; i >= 0 ; i--)
        delete topLevelItem(i);

    foreach (QString path, qApp->workSpace()->librariesPaths) {
        if(!path.isEmpty())
        {
            QDir dir(path);
            if ( dir.exists() ){
                path.remove("\r");
                ComponentsTreeWidgetItem *item = new ComponentsTreeWidgetItem(this,1002,path);
                item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
                addTopLevelItem(item);
            }
        }
    }
}

void ComponentsTreeWidget::onExpand(QTreeWidgetItem *item){
    if ( !item->childCount() ){
        ComponentsTreeWidgetItem *componentsItem = dynamic_cast<ComponentsTreeWidgetItem *>(item);
        if ( ! componentsItem )
            return;

        QFileInfoList subFolders = componentsItem->dir().entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

        foreach (QFileInfo folderName, subFolders){
            ComponentsTreeWidgetItem *it = new ComponentsTreeWidgetItem(componentsItem,1002,folderName.absoluteFilePath());
            it->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
            item->addChild(it);
        }

        subFolders = componentsItem->dir().entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

        foreach (QFileInfo folderName, subFolders){
            if ( folderName.suffix() == "mtc" ){
                ComponentsTreeWidgetItem *it = new ComponentsTreeWidgetItem(componentsItem,1002,folderName.absoluteFilePath());
                it->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
                item->addChild(it);
            }
        }
    }
}
