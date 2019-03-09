#ifndef SELECTTOOL_H
#define SELECTTOOL_H

/*
 * Tool that handles selection behaviour
 */

#include <QWidget>
#include "../mytoolbase.h"
#include "items/myitembase.h"
#include "moveundocommand.h"
class Angle;
class Track;

namespace Ui {
class SelectTool;
}

class SelectTool : public MYToolBase
{
    Q_OBJECT

public:
    explicit SelectTool(QWidget *parent = 0, QMenu *contextMenu = 0);
    ~SelectTool();

    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleWheelEvent(QWheelEvent *event);
    void handleKeyPressEvent(QKeyEvent *event);
    void handleKeyReleaseEvent(QKeyEvent *event);
    void handleEnterEvent(QEvent *event);
    void handleLeaveEvent(QEvent *event);

signals:
    void showOptionsSignal();

public slots:
    void updateList();
    void alignSelectedAngle();
    void alignAngles();
    void splitTrack();
    void mergeTracks();
    void insertAngle();

private slots:
    void removeAngle();

private:
    Ui::SelectTool *ui;
    QList<MYItemBase *> m_itemsInBoundingRect;
    MYItemBase *typonItemAt(QPointF pos, QTransform transform);
    void aligneAngle(Angle *angle);

    QMenu *m_contextMenu;
    bool m_isReadyToDragItems;
    bool m_isReadyToDragAngle;
    Angle *m_draggingAngle;
    QPointF m_oldAnglePos;
    QPointF m_lastPos;
    QPointF m_totalDelta;
    Angle *m_selectedAngle;
    Track *m_selectedTrack;
    QPoint m_insertPos;
};

#endif // SELECTTOOL_H
