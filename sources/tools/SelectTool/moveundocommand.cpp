#include "moveundocommand.h"
#include "global.h"
#include <QDebug>

MoveUndoCommand::MoveUndoCommand(QList<MYItemBase *> items, QPointF delta){
    setText(QObject::tr("Move"));
    m_items = items;
    m_delta = delta;
    m_firstRedo = true;
}

// return items boundingrect in order to update groundplanes
QRectF MoveUndoCommand::itemsBoundingRect(){
    QRectF bRect = QRectF();
    foreach(MYItemBase* item, m_items)
        bRect = bRect.united(item->groupBoundingRect());
    return bRect;

}

void MoveUndoCommand::undo(){
    foreach (MYItemBase *it, m_items )
        it->moveItemBy(-m_delta);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(itemsBoundingRect());
}

void MoveUndoCommand::redo(){
    if ( !m_firstRedo ){
        foreach (MYItemBase *it, m_items )
            it->moveItemBy(m_delta);
    }else{
        m_firstRedo = false;
    }
    qApp->currentTypon()->typonScene()->updateGroundPlanes(itemsBoundingRect());
}
