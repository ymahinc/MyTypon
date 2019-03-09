#ifndef MYTOOLBASE_H
#define MYTOOLBASE_H

/*
 * Class that handles basic mouse behaviour:
 * Rubberband rect in scene coordiantes
 * autoscroll view when mouse get outside viewport if desired
 */

#include <QWidget>
#include <QIcon>
#include <QMouseEvent>
#include <QMap>
#include <QPainterPath>
#include <QColor>
#include <QTimer>
#include <QEvent>
#include <QPoint>
#include "MoveTool/movetoolcore.h"
class MYItemBase;

namespace Ui {
class MYToolBase;
}

class MYToolBase : public QWidget
{
    Q_OBJECT

public:
    enum AutoExpandMode{Normal, // normal = autoexpand only if mouse left button is pressed
                        Auto,   // Auto = autoexpand until mouse right button is pressed
                        Auto2}; // Auto = autoexpand until mouse left button is pressed a second time


    explicit MYToolBase(QWidget *parent = 0);
    ~MYToolBase();

    void setAcceptWheelEvents(bool accept);
    bool acceptWheelEvents();
    void setAcceptRubberband(bool accept);
    bool acceptRubberBand();
    void setAcceptAutoExpand(bool accept, AutoExpandMode mode = Normal);
    bool acceptAutoExpand();
    void setToolName(QString name);
    QString toolName();
    void setToolIcon(QIcon icon);
    QIcon toolIcon();
    bool needToBeDrawn();
    bool needToDrawRubberBand();
    QRectF rubberBandRect();
    QCursor cursor();
    QPainterPath path();
    bool midZoomModOn();

    void invalidateRect(QRectF r);
    virtual void handleMousePressEvent(QMouseEvent *event) = 0;
    virtual void handleMouseReleaseEvent(QMouseEvent *event) = 0;
    virtual void handleMouseMoveEvent(QMouseEvent *event) = 0;
    virtual void handleWheelEvent(QWheelEvent * event) = 0;
    virtual void handleEnterEvent(QEvent * event) = 0;
    virtual void handleLeaveEvent(QEvent * event) = 0;
    virtual void handleKeyPressEvent(QKeyEvent *event) = 0;
    virtual void handleKeyReleaseEvent(QKeyEvent *event) = 0;

signals:
    void nameChanged(MYItemBase *item);

protected:
    void mousePressEventTool(QMouseEvent *event);
    void mouseReleaseEventTool(QMouseEvent *event);
    void mouseMoveEventTool(QMouseEvent *event);
    void mouseWheelEventTool(QWheelEvent *event);
    void keyPressEventTool(QKeyEvent *event);
    void keyReleaseEventTool(QKeyEvent *event);
    void enterEventTool(QEvent *event);
    void leaveEventTool(QEvent *event);

    void startExtending();
    void stopExtending();
    QTimer *m_extendingTimer;
    bool isExtending;

    bool m_isRubberBanding;
    QPointF m_lastPosScene;
    bool m_needToBeDrawn;
    QRectF m_rubberBandRect;
    QCursor m_cursor;
    QPainterPath m_path;
    QRectF m_boundingRect;
    bool m_isReadyToExpand;

    bool m_ctrlKeyIsPressed;

private slots:
    void extend();

private:
    Ui::MYToolBase *ui;
    void zoomWheel(bool in);

    bool m_acceptWheelEvents;
    bool m_acceptRubberBand;
    bool m_acceptAutoExpand;

    QString m_toolName;
    QIcon m_icon;
    MoveToolCore *m_midMoveTool;
    AutoExpandMode m_autoExpandMode;
};

#endif // MYTOOLBASE_H
