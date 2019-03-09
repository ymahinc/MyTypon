#ifndef MOVETOOLCORE_H
#define MOVETOOLCORE_H

/*
 * Base tool to move around the scene
 * used by mid button
 * and left button if moveTool is the current tool
 */

#include <QObject>
#include <QMouseEvent>
#include <QCursor>

class MoveToolCore : public QObject
{
    Q_OBJECT

public:
    MoveToolCore(QObject * parent = 0, QCursor *oldCursor = 0);
    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void setDragging(bool on);
    bool isDragging();

private:
    bool m_isDragging;
    QCursor *m_oldCursor;
};

#endif // MOVETOOLCORE_H
