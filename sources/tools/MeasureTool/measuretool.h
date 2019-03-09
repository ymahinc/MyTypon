#ifndef MEASURETOOL_H
#define MEASURETOOL_H

/*
 * Tool that provide a meanig to measure distances on the typon scene
 * draw a path on the scene representing distance and angle
 * provide a widget that display measured values
 */

#include <QWidget>
#include "../mytoolbase.h"

namespace Ui {
class MeasureTool;
}

class MeasureTool : public MYToolBase
{
    Q_OBJECT

public:
    explicit MeasureTool(QWidget *parent = 0);
    ~MeasureTool();

    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleWheelEvent(QWheelEvent *event);
    void handleKeyPressEvent(QKeyEvent *event);
    void handleKeyReleaseEvent(QKeyEvent *event);
    void handleEnterEvent(QEvent *event);
    void handleLeaveEvent(QEvent *event);

private:
    void updateBoundingRect();
    void updatePath(QPoint point);
    void updateWidgets();

    Ui::MeasureTool *ui;
    QPointF m_firstPoint;
    QPointF m_secondPoint;
    bool m_isMeasuring;
};

#endif // MEASURETOOL_H
