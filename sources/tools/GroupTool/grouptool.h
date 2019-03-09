#ifndef GROUPTOOL_H
#define GROUPTOOL_H

#include <QWidget>
#include "../mytoolbase.h"

namespace Ui {
class GroupTool;
}

class GroupTool : public MYToolBase
{
    Q_OBJECT

public:
    explicit GroupTool(QWidget *parent = 0, MYItemBase *item = 0);
    ~GroupTool();

    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event) ;
    void handleWheelEvent(QWheelEvent * event);
    void handleEnterEvent(QEvent * event);
    void handleLeaveEvent(QEvent * event);
    void handleKeyPressEvent(QKeyEvent *event);
    void handleKeyReleaseEvent(QKeyEvent *event);

    void updateGroup();

private slots:
    void onNameChanged();

private:
    Ui::GroupTool *ui;

    MYItemBase *m_group;
};

#endif // GROUPTOOL_H
