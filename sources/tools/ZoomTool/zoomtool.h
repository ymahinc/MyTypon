#ifndef ZOOMTOOL_H
#define ZOOMTOOL_H

/*
 * Tool that provides zooming functionalities
 */

#include <QWidget>
#include "../mytoolbase.h"

namespace Ui {
class ZoomTool;
}

class ZoomTool : public MYToolBase
{
    Q_OBJECT

public:
    explicit ZoomTool(QWidget *parent = 0);
    ~ZoomTool();

    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleWheelEvent(QWheelEvent *event);
    void handleKeyPressEvent(QKeyEvent *event);
    void handleKeyReleaseEvent(QKeyEvent *event);
    void handleEnterEvent(QEvent *event);
    void handleLeaveEvent(QEvent *event);

signals:
    void zoomIn();
    void zoomOut();
    void zoomReset();
    void zoomBoard();
    void zoomRect(QRectF rect, bool in);
    void zoomFit();
    void zoomSelection();

public slots:
    void updateWidgets();

private:
    Ui::ZoomTool *ui;

    bool m_isZoomingIn;
};

#endif // ZOOMTOOL_H
