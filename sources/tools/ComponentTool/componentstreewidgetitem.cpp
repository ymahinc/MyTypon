#include "componentstreewidgetitem.h"
#include <QApplication>
#include <QDebug>

ComponentsTreeWidgetItem::ComponentsTreeWidgetItem(QTreeWidget *parent, int type, QString path)
    :  QTreeWidgetItem(parent, type){
    init(path);
}

ComponentsTreeWidgetItem::ComponentsTreeWidgetItem(ComponentsTreeWidgetItem *parent, int type, QString path)
    :  QTreeWidgetItem(parent, type){
    init(path);
}

void ComponentsTreeWidgetItem::init(QString path){
    QFileInfo fileInfo(path);
    QString fileName = fileInfo.baseName();
    if ( fileName.isEmpty() )
        fileName = "." + fileInfo.suffix();
    setText(0,fileName);
    m_isFile = fileInfo.isFile();
    if ( m_isFile )
        setIcon(0,QIcon(":/pictures/chip2.png"));
    else
        setIcon(0,QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    m_path = path;
}

QDir ComponentsTreeWidgetItem::dir(){
    QDir dir(m_path);
    return dir;
}
