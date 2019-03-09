#include "componentslistwidgetitem.h"
#include <QFileInfo>

ComponentsListWidgetItem::ComponentsListWidgetItem(QListWidget *parent, int type, QString path)
    : QListWidgetItem(parent,type), m_path(path){
    setText(QFileInfo(path).baseName());
    setIcon(QIcon(":/pictures/chip2.png"));
}

QString ComponentsListWidgetItem::path(){
    return m_path;
}

