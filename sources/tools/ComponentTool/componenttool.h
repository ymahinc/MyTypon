#ifndef COMPONENTTOOL_H
#define COMPONENTTOOL_H

#include <QWidget>
#include "../mytoolbase.h"
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

namespace Ui {
class ComponentTool;
}

class ComponentTool : public MYToolBase
{
    Q_OBJECT

public:
    explicit ComponentTool(QWidget *parent = 0);
    ~ComponentTool();

    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleWheelEvent(QWheelEvent *event);
    void handleKeyPressEvent(QKeyEvent *event);
    void handleKeyReleaseEvent(QKeyEvent *event);
    void handleEnterEvent(QEvent *event);
    void handleLeaveEvent(QEvent *event);

private slots:
    void updateFilter();

private:
    Ui::ComponentTool *ui;
};

#endif // COMPONENTTOOL_H
