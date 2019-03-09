#ifndef COMPONENTSLISTWIDGETITEM_H
#define COMPONENTSLISTWIDGETITEM_H

#include <QListWidgetItem>

class ComponentsListWidgetItem : public QListWidgetItem
{
public:
    ComponentsListWidgetItem(QListWidget *parent = Q_NULLPTR, int type = Type, QString path = QString());
    QString path();

private:
    QString m_path;
};

#endif // COMPONENTSLISTWIDGETITEM_H
