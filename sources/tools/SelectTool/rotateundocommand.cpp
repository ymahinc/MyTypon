#include "rotateundocommand.h"
#include <QDebug>
#include "global.h"

RotateUndoCommand::RotateUndoCommand(QList<MYItemBase *> items, double angle){
    setText(QObject::tr("Rotate"));
    m_items = items;
    m_angle = angle;
}

// get bounding rect for all items to rotate
// in order to update groundplanes and find rotate point
QRectF RotateUndoCommand::itemsBoundingRect(){
    QRectF bRect = QRectF();
    foreach(MYItemBase* item, m_items)
        bRect = bRect.united(item->groupBoundingRect());
    return bRect;

}

void RotateUndoCommand::undo(){
    QRectF rect;
    foreach(MYItemBase* item, m_items)
        rect = rect.united(item->groupBoundingRect());

    QPointF center = rect.center();

    foreach(MYItemBase* item, m_items)
        item->rotate(-m_angle, center);

    qApp->currentTypon()->typonScene()->updateGroundPlanes(itemsBoundingRect());
}

void RotateUndoCommand::redo(){
    QRectF rect;
    foreach(MYItemBase* item, m_items)
        rect = rect.united(item->groupBoundingRect());

    QPointF center = rect.center();

    foreach(MYItemBase* item, m_items)
        item->rotate(m_angle, center);

    qApp->currentTypon()->typonScene()->updateGroundPlanes(itemsBoundingRect());
}
