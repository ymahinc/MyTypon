#ifndef TRACKTOOL_H
#define TRACKTOOL_H

#include <QWidget>
#include "../mytoolbase.h"
#include "items/trackitem/track.h"
#include <QVector>

namespace Ui {
class TrackTool;
}

class TrackTool : public MYToolBase
{
    Q_OBJECT

public:
    enum Mode{Simple, OneAngle1, OneAngle2, RightAngle1, RightAngle2, Curved, Curved2};

    explicit TrackTool(QWidget *parent = 0, Track *item = 0);
    ~TrackTool();

    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleWheelEvent(QWheelEvent *event);
    void handleKeyPressEvent(QKeyEvent *event);
    void handleKeyReleaseEvent(QKeyEvent *event);
    void setMode(Mode mode);
    void handleEnterEvent(QEvent *event);
    void handleLeaveEvent(QEvent *event);

    void updateTool();

public slots:
    void updateCurrentMax();
    void updateWidth();
    void updateTrackType(int index);
    void swicthToNextType();
    void swicthToPreviousType();
    void updateBus();

private slots:
    void updateBusWidgets();
    void onNameChanged();

private:
    Ui::TrackTool *ui;
    void addTrackToTypon();

    void updateSecondPoint(Track *track, QPoint pos, Angle *firstAngle, Angle *midAngle, Angle *lastAngle, int index = -1);
    void clearBusVectors();    

    bool m_isCreatingTrack;
    Track *m_track;
    Track *m_tempTrack;
    Angle *m_firstAngle;
    Angle *m_midAngle;
    Angle *m_lastAngle;
    Mode m_mode;
    Track::TrackType m_trackType;
    QVector<Track *> m_busTrackVector;
    QVector<Track *> m_busTempTrackVector;
    QVector<Angle *> m_busFirstAngleVector;
    QVector<Angle *> m_busMidAngleVector;
    QVector<Angle *> m_busLastAngleVector;
    bool m_busVisible;
};

#endif // TRACKTOOL_H
