#ifndef MINIVIEW_H
#define MINIVIEW_H

/* graphicssview that present a global view of current typon
 * We can navigate through current typon by dragging in this view
 * */

#include <QGraphicsView>
#include "typonWidget/mytyponscene.h"
#include "typonWidget/mytyponview.h"

class MiniView : public QGraphicsView
{
    Q_OBJECT

public: 
    MiniView(QWidget *parent = Q_NULLPTR);
    MiniView(QGraphicsScene *scene, QWidget *parent = Q_NULLPTR);
    void updateTypon();

public slots:
    void invalidate();

protected:
    void drawBackground(QPainter* painter, const QRectF &rect);
    void resizeEvent(QResizeEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private:
    void init();
    bool m_isMoving;
};

#endif // MINIVIEW_H
