#ifndef LAYER_H
#define LAYER_H

// class wich handles all layers properties

#include <QColor>
#include <QRectF>
#include <QGraphicsItem>
class MYItemBase;

class Layer : public QGraphicsItem
{

public:
    enum { Type = UserType + 1 };

    Layer(QGraphicsItem * parent, QString name = QString(), QColor color = Qt::blue);
    ~Layer();

    int type() const;
    void setColor(QColor color);
    QColor color();
    void setName(QString name);
    QString name();
    void setSystem(bool systemLayer);
    bool isSystem();
    int layerZValue();
    int realLayerZValue();
    Layer *clone(QGraphicsItem *parent = Q_NULLPTR);
    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    void updateZValue();
    void updateTransparency();
    void setInternal(bool internal);
    bool isInternal();
    QList<MYItemBase *> *groundPlanedItems();
    void updateGroundPlanes(QRectF rect);
    void clearSelection();
    void save(QDataStream &stream);

private:
    QString m_layerName;
    QColor m_color;
    bool m_isSystemLayer;
    bool m_isActive;
    QList<MYItemBase *> *m_groundPlanedItems;
};

#endif // LAYER_H
