#ifndef ANGLE_H
#define ANGLE_H

#include "items/myitembase.h"

class Angle : public MYItemBase
{
public:
    Angle(QGraphicsItem * parent = 0);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    virtual void updateGeometry();
    void setWidth(double width);
    double width();
    QPainterPath shape() const;
    void save(QDataStream &stream);
    int restore(QDataStream &stream);

private:
    QRectF m_bRect;
    double m_width;
    QPainterPath m_shape;
};

#endif // ANGLE_H
