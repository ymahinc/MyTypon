#ifndef MOVETOOL_H
#define MOVETOOL_H

/*
 * Tool used for dragging around the scene with left button
 */

#include <QWidget>
#include "../mytoolbase.h"
#include "movetoolcore.h"

namespace Ui {
class MoveTool;
}

class MoveTool : public MYToolBase
{
    Q_OBJECT

public:
    explicit MoveTool(QWidget *parent = 0);
    ~MoveTool();

    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleWheelEvent(QWheelEvent *event);
    void handleKeyPressEvent(QKeyEvent *event);
    void handleKeyReleaseEvent(QKeyEvent *event);
    void handleEnterEvent(QEvent *event);
    void handleLeaveEvent(QEvent *event);

private:
    Ui::MoveTool *ui;

    MoveToolCore *m_moveCore;
};

#endif // MOVETOOL_H
