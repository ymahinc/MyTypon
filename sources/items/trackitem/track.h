#ifndef TRACK_H
#define TRACK_H

#include "items/myitembase.h"
class Angle;
class TrackTool;

class Track : public MYItemBase
{
public:
    enum TrackType{Flat, Curved, Curved2};

    Track(MYItemBase *parent = 0, bool incrementCounter = false);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    virtual void updateGeometry();
    void setWidth(double width);
    double width();
    QVector<Angle *> angles();
    void setType(TrackType type);
    TrackType trackType();
    QPainterPath shape() const;
    virtual void appendAngle(QPointF pos, bool checkLastPos = true);
    void appendAngle(Angle *angle, bool checkLastPos = true);
    void prependAngle(Angle *angle, bool checkLastPos = true);
    virtual void removeAngle(Angle *angle, bool deleteAfterRemove = true);
    void clearTrack();
    virtual void updateAnglePos(Angle *angle, QPointF pos);
    Track *clone();
    void append(Track *track, bool reversed = false);
    void setTool(TrackTool *tool);
    TrackTool *tool();
    void onTyponSelectedChange();
    void hideAngles();
    void onAddToGroup();
    void insertAngle(Angle *angle, int index, QPointF pos);
    bool angleIsOnExtremity(Angle *angle);
    bool isFirstAngle(Angle *angle);
    bool isLastAngle(Angle *angle);
    int indexOfPoint(QPointF point);
    QPointF linePoint(QPointF point, int index = -1);
    void save(QDataStream &stream);
    int restore(QDataStream &stream);

protected:
    QVector<Angle *> m_angles;
    void generatePath(QPainterPath &path, Angle *angle1, Angle *angle2);
    double m_width;

private:   
    QRectF m_bRect;    
    QPainterPath m_shape;
    QPainterPath m_path;
    TrackType m_type;    
    TrackTool *m_tool;
};

#endif // TRACK_H
