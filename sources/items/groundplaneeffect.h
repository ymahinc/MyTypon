#ifndef GROUNDPLANEEFFECT_H
#define GROUNDPLANEEFFECT_H

#include <QGraphicsEffect>
#include <QRectF>

class GroundPlaneEffect : public QGraphicsEffect
{
    Q_OBJECT

public:
    GroundPlaneEffect();
    virtual QRectF boundingRectFor(const QRectF& sourceRect) const;
    void setOpacityMask(QBrush mask);

protected:
    virtual void draw(QPainter *painter); // , QGraphicsEffectSource *source );

private:
    QBrush m_brush;
};

#endif // GROUNDPLANEEFFECT_H
