#include "mirrorundocommand.h"
#include "global.h"

MirrorUndoCommand::MirrorUndoCommand(QList<MYItemBase *> items, bool horizontal){
    setText(QObject::tr("Mirror"));
    m_items = items;
    m_horizontal = horizontal;
}

// get bounding rect for all items to rotate
// in order to update groundplanes and find mirror point
QRectF MirrorUndoCommand::itemsBoundingRect(){
    QRectF bRect = QRectF();
    foreach(MYItemBase* item, m_items)
        bRect = bRect.united(item->groupBoundingRect());
    return bRect;

}

void MirrorUndoCommand::undo(){
    QRectF rect;
    foreach(MYItemBase* item, m_items)
        rect = rect.united(item->groupBoundingRect());

    QPointF center = rect.center();

    foreach(MYItemBase* item, m_items){
        if ( m_horizontal )
            item->horizontalMirror(center);
        else
            item->verticalMirror(center);
    }
    qApp->currentTypon()->typonScene()->updateGroundPlanes(itemsBoundingRect());
}

void MirrorUndoCommand::redo(){
    QRectF rect;
    foreach(MYItemBase* item, m_items)
        rect = rect.united(item->groupBoundingRect());

    QPointF center = rect.center();

    foreach(MYItemBase* item, m_items){
        if ( m_horizontal )
            item->horizontalMirror(center);
        else
            item->verticalMirror(center);
    }
    qApp->currentTypon()->typonScene()->updateGroundPlanes(itemsBoundingRect());
}
