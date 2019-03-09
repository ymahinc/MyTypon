#include "addtextundocommand.h"
#include "global.h"
#include <QDebug>

AddTextUndoCommand::AddTextUndoCommand(TextItem *item){
    setText(QObject::tr("Add Text"));
    m_textItem = item->clone();
    m_textItem->setBold(item->bold());
    m_textItem->setItalic(item->italic());
    m_textItem->setUnderlined(item->underlined());
    m_textItem->setText(item->text());
    m_textItem->setWeight(item->weight());
    m_textItem->setFont(item->font());
    m_textItem->setTransform(item->transform());
    m_textItem->setRotation(item->rotation());
    m_layer = item->layer();
    m_pos = item->pos();
}

AddTextUndoCommand::AddTextUndoCommand(QDataStream &stream, MYItemBase *group){
    setText(QObject::tr("Add Text"));
    m_textItem = new TextItem(0);
    int layerZValue = m_textItem->restore(stream);
    m_layer = qApp->currentTypon()->typonScene()->layers()->layerFromZValue(layerZValue);
    m_pos = m_textItem->pos();
    if ( group )
        group->addToGroup(m_textItem);
}

AddTextUndoCommand::~AddTextUndoCommand(){
    if ( ! m_textItem->parentItem() )
        delete m_textItem;
}

void AddTextUndoCommand::undo(){
    qApp->currentTypon()->typonScene()->removeTyponItem(m_textItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_textItem->groupBoundingRect(),m_layer);
}

void AddTextUndoCommand::redo(){
    m_textItem->setParentItem(m_layer);
    m_textItem->setPos(m_pos);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_textItem->groupBoundingRect(),m_layer);
}
