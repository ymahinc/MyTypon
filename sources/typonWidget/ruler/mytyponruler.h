#ifndef MYTYPONRULER_H
#define MYTYPONRULER_H

/*
 * widget wih displaya ruler in a typonwidget
 * Can be horizontal or vertiical
 */

#include <QGraphicsView>
#include <QObject>

class MYTyponScene;

class MYTyponRuler : public QGraphicsView
{
    Q_OBJECT

public:
    enum Orientations{ Horizontal, Vertical };

    MYTyponRuler(MYTyponScene *typonScene, QWidget * parent = 0, Orientations orientation =  Horizontal);
    int decNumber();

public slots:
    void updatePos(QPointF pos);
    void hideLines();
    void changeVisibility();
    void refresh();

protected:
    void drawBackground (QPainter* painter, const QRectF &rect);
    void drawForeground(QPainter* painter, const QRectF &rect);

private:
    void init();

    Orientations m_orientation;
    QGraphicsScene *m_scene;
    QPointF m_lasMousePos;

    bool m_linesVisible;    
    int m_width;
    int m_height;
};

#endif // MYTYPONRULER_H
