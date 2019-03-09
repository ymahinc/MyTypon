#ifndef MYTYPONVIEW_H
#define MYTYPONVIEW_H

// drawing zone of a typon widget

#include <QGraphicsView>
#include <QObject>
class MYTyponScene;

class MYTyponView : public QGraphicsView
{
    Q_OBJECT

public:
    MYTyponView(MYTyponScene * scene, QWidget * parent = 0);
    ~MYTyponView();

    QPoint lastMousePos();
    bool cursorIsInViewport();
    void leaveViewport(QEvent *event = 0);
    double levelOfdetails();
    void updateLastMousePos(bool extending = false); // update lines coordinates after scaling the view
    QRectF sceneViewportRect();
    MYTyponScene *typonScene();
    bool canZoomIn();
    bool canZoomOut();
    void momentaryDisableSnap(bool disable);
    QRectF typonRect();
    QPointF nearestGridPoint(QPointF point);

signals:
    void mousePosHasChangedScenePos(QPointF pos);
    void viewportLeaved();

protected:
    void mouseMoveEvent(QMouseEvent *mouseEvent);
    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent * event);
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);
    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event);
    void drawBackground(QPainter* painter, const QRectF &rect);
    void drawForeground(QPainter* painter, const QRectF &rect);

private:
    void init();
    void updateMousePos(QPoint pos, bool fromWheel = false);
    void drawBackgroundContent(QPainter* painter, const QRectF &rect);

    QPoint m_lastMousePos;
    QPointF m_lastMouseScenePos;
    MYTyponScene *m_scene;
    bool m_cursorIsInViewPort;
    bool m_momentaryDisableSnap;
    bool m_cursorIsSet;
    double m_dynamicGridFactor;
    bool m_cursorHasQuitViewport;
};

#endif // MYTYPONVIEW_H
