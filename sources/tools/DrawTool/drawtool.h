#ifndef DRAWTOOL_H
#define DRAWTOOL_H

#include <QWidget>
#include "../mytoolbase.h"
/*
#include "items/rectangleitem/rectangleitem.h"
#include "items/ellipseitem/ellipseitem.h"
#include "items/polygonitem/polygonitem.h"*/
class DrawItem;

namespace Ui {
class DrawTool;
}

class DrawTool : public MYToolBase
{
    Q_OBJECT

public:
    explicit DrawTool(QWidget *parent = 0, DrawItem *drawItem = 0);
    ~DrawTool();
    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleWheelEvent(QWheelEvent *event);
    void handleKeyPressEvent(QKeyEvent *event);
    void handleKeyReleaseEvent(QKeyEvent *event);
    void handleEnterEvent(QEvent *event);
    void handleLeaveEvent(QEvent *event);
    void updateTool();

private slots:
    void updateWidth();
    void updateDrawType(int index);
    void updateFillType(int index);
    void updateOffset();
    void onNameChanged();

private:
    Ui::DrawTool *ui;
    void stopDrawing();

   /* RectangleItem *m_rectItem;
    EllipseItem *m_ellipseItem;
    PolygonItem *m_polygonItem;*/
    DrawItem *m_drawItem;
    bool m_isDrawing;
};

#endif // DRAWTOOL_H
