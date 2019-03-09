#ifndef PADTOOL_H
#define PADTOOL_H

#include <QWidget>
#include "../mytoolbase.h"
#include "../../items/paditem/padgroup.h"

namespace Ui {
class PadTool;
}

class PadTool : public MYToolBase
{
    Q_OBJECT

public:
    explicit PadTool(QWidget *parent = 0, PadGroup *item = 0);
    ~PadTool();

    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleWheelEvent(QWheelEvent *event);
    void handleKeyPressEvent(QKeyEvent *event);
    void handleKeyReleaseEvent(QKeyEvent *event);
    void handleEnterEvent(QEvent *event);
    void handleLeaveEvent(QEvent *event);

    void updateTool();

private slots:
    void updatePadType(int index);
    void updateHole();
    void updateWidth();
    void updateHoleWidth();
    void updateVia();
    void updateBus();
    void onNameChanged();

private:
    void swicthToNextType();
    void swicthToPreviousType();   
    void clearBus();

    Ui::PadTool *ui;
    PadGroup *m_group;
    bool m_holeWasActivated;
    QVector<PadGroup *> m_bus;
};

#endif // PADTOOL_H
