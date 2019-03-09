#ifndef PADITEM_H
#define PADITEM_H

#include "../myitembase.h"
#include "holeitem.h"

class PadItem : public MYItemBase
{
public:
    PadItem(MYItemBase *parent = 0);
    ~PadItem();

    enum PadType{Round, Square, RoundSquare, PolySquare, Ellipse, RoundRectangle, Rectangle, Polygon, Hole};

    void setWidth(double width);
    double width();
    PadItem *clone();
    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    QPainterPath shape() const;
    void setType(PadType type);
    PadType padType();
    void setHole(HoleItem *hole);
    void updateGeometry();
    HoleItem *hole();

private:    
    QRectF m_bRect;
    QPainterPath m_shape;
    double m_width;
    PadType m_type;
    QPainterPath m_path;
    QPainterPath m_holePath;
    HoleItem *m_hole;
};

#endif // PADITEM_H
