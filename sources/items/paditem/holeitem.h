#ifndef HOLEITEM_H
#define HOLEITEM_H

#include "../myitembase.h"

class HoleItem : public MYItemBase
{
public:
    HoleItem(MYItemBase *parent = 0);
    void setWidth(double width);
    double width();
    HoleItem *clone();
    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    QPainterPath shape() const;

private:
    void updateGeometry();
    QRectF m_bRect;
    QPainterPath m_shape;
    QPainterPath m_path;
    double m_width;
};

#endif // HOLEITEM_H
