#ifndef DRAWITEM_H
#define DRAWITEM_H

#include "items/trackitem/track.h"
class DrawTool;

class DrawItem : public Track
{
public:
    enum FillType{NoFill, FullFill, GroundPlane};
    enum DrawType{Rectangle, Polygon, Ellipse};

    DrawItem(MYItemBase *parent = 0);

    DrawItem *clone();
    void setFillType(FillType type);
    FillType fillType();
    void setTool(DrawTool *tool);
    DrawTool *tool();
    QPainterPath shape() const;
    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    void updateGeometry();
    DrawType drawType();
    void setDrawType(DrawType drawType);
    virtual void removeAngle(Angle *angle, bool deleteAfterRemove = true);
    virtual void updateAnglePos(Angle *angle, QPointF pos);
    virtual void appendAngle(QPointF pos, bool checkLastPos = true);
    void save(QDataStream &stream);
    int restore(QDataStream &stream);

private:
    FillType m_fillType;
    DrawTool *m_tool;
    QRectF m_bRect;
    QPainterPath m_shape;
    QVector<QPainterPath> m_paths;
    DrawType m_drawType;
};

#endif // DRAWITEM_H
