#ifndef COMPONENTSTREEWIDGETITEM_H
#define COMPONENTSTREEWIDGETITEM_H

#include <QTreeWidgetItem>
#include <QFileInfo>
#include <QDir>

class ComponentsTreeWidgetItem : public QTreeWidgetItem
{
public:
    ComponentsTreeWidgetItem(QTreeWidget *parent, int type = 1002, QString path = QString());
    ComponentsTreeWidgetItem(ComponentsTreeWidgetItem *parent, int type = 1002, QString path = QString());
    QDir dir();

private:
    void init(QString path);

    QString m_path;
    bool m_isFile;
};

#endif // COMPONENTSTREEWIDGETITEM_H
