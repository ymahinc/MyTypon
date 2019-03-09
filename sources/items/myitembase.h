#ifndef MYITEMBASE_H
#define MYITEMBASE_H

/*
 * base class for all typon items
 * Handle custom selection
 * Can be a group item, group items are never added to the scene, they only handles children list and rotate, mirror and move it accordingly
 */

#include <QGraphicsItem>
#include "layers/layer.h"
#include "groundplaneeffect.h"
#include "tools/GroupTool/grouptool.h"
class MYToolBase;

class MYItemBase : public QGraphicsItem
{
public:
    enum ItemType{Base, LayerId, TrackContain, Track, Rectangle, Ellipse, Polygon, Pad, Hole, Text};

    MYItemBase(QGraphicsItem * parent = 0, bool isGroup = false);
    virtual ~MYItemBase();
    QString itemName();
    void setItemName(QString name);
    Layer *layer() const;
    void setLayer(Layer *layer);
    bool isGroundPlane() const;
    void setGroundPlane(bool groundPlane);
    virtual void updateGeometry();
    void setGroundPlaneOffset(double offset);
    double groundPlaneOffset();
    bool isToolItem();
    void setToolItem(bool toolItem);
    QVector<QPainterPath> collidingPaths();
    void setTyponItemSelected(bool selected);
    bool typonItemIsSelected();
    void setGroup(MYItemBase *group);
    MYItemBase *group();
    MYItemBase *topMostGroup();
    QList<MYItemBase *> childs();
    void setUngroupable(bool ungroupable);
    bool isUngroupable();
    void addToGroup(MYItemBase *item);
    void removeFromGroup(MYItemBase *item);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    bool isGroup();
    void setGroupTool(GroupTool *tool);
    GroupTool *groupTool();
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    bool allChildsAreUnavailable();
    bool isOffScreen();
    void setOffScreen(bool offScreen);
    QRectF groupBoundingRect();
    void rotate(double angle, QPointF center);
    void horizontalMirror(QPointF center);
    void verticalMirror(QPointF center);
    void alignOnGrid();
    void moveItemBy(QPointF delta);
    void setGroupPos(QPointF point);
    QPointF groupPos();
    bool isAngle();
    virtual void onTyponSelectedChange();
    virtual void onAddToGroup();
    double originalZValue();
    virtual void save(QDataStream &stream);
    virtual int restore(QDataStream &stream);

protected:
    QString m_name; 
    bool m_isSelected;
    bool m_isAngle;
    double m_originalZValue;

private:
    bool m_isGroundPlane;
    double m_groundPlaneOffset = 50;
    bool m_isToolItem;
    GroundPlaneEffect *m_gndPlaneEffect;    
    QList<MYItemBase *> m_childs;
    bool m_isUngroupable;
    MYItemBase *m_group;
    bool m_isGroup;
    GroupTool *m_tool;
    bool m_isOffScreen;
    QPointF m_groupHandlePoint;
};

#endif // MYITEMBASE_H
