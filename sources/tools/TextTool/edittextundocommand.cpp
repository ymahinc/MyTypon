#include "edittextundocommand.h"
#include <QDebug>
#include "tools/TextTool/texttool.h"
#include "global.h"

// store all properties of the modified item in a temporary textItem
EditTextUndoCommand::EditTextUndoCommand(TextItem *item, QString itemName, QString itemText, QFont font, double weight){
    setText(QObject::tr("Edit Text"));
    m_targetItem = item;
    m_newTextItem = new TextItem();

    m_newTextItem->setItemName(itemName);
    m_newTextItem->setBold(font.bold());
    m_newTextItem->setItalic(font.italic());
    m_newTextItem->setUnderlined(font.underline());
    m_newTextItem->setFont(font);
    m_newTextItem->setWeight(weight);
    m_newTextItem->setText(itemText);

    m_oldTextItem.setItemName(item->itemName());
    m_oldTextItem.setBold(item->bold());
    m_oldTextItem.setItalic(item->italic());
    m_oldTextItem.setUnderlined(item->underlined());
    m_oldTextItem.setFont(item->font());
    m_oldTextItem.setWeight(item->width());
    m_oldTextItem.setText(item->text());
}

EditTextUndoCommand::~EditTextUndoCommand(){
    delete m_newTextItem;
}

// restore old values for the item
void EditTextUndoCommand::undo(){
    m_targetItem->setItemName(m_oldTextItem.itemName());
    m_targetItem->setBold(m_oldTextItem.bold());
    m_targetItem->setItalic(m_oldTextItem.italic());
    m_targetItem->setUnderlined(m_oldTextItem.underlined());
    m_targetItem->setFont(m_oldTextItem.font());
    m_targetItem->setWeight(m_oldTextItem.width());
    m_targetItem->setText(m_oldTextItem.text());
    // update widget values if there is one
    if ( m_targetItem->tool() )
         m_targetItem->tool()->updateTool();
    qApp->selectionTree()->updateName(m_targetItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_targetItem->groupBoundingRect(),m_targetItem->layer());
}

// restore new values for the item
void EditTextUndoCommand::redo(){
    m_targetItem->setItemName(m_newTextItem->itemName());
    m_targetItem->setBold(m_newTextItem->bold());
    m_targetItem->setItalic(m_newTextItem->italic());
    m_targetItem->setUnderlined(m_newTextItem->underlined());
    m_targetItem->setFont(m_newTextItem->font());
    m_targetItem->setWeight(m_newTextItem->width());
    m_targetItem->setText(m_newTextItem->text());
    // update widget values if there is one
    if ( m_targetItem->tool() )
         m_targetItem->tool()->updateTool();
    qApp->selectionTree()->updateName(m_targetItem);
    qApp->currentTypon()->typonScene()->updateGroundPlanes(m_targetItem->groupBoundingRect(),m_targetItem->layer());
}

TextItem *EditTextUndoCommand::textItem() const{
    return m_newTextItem;
}

TextItem *EditTextUndoCommand::sourceItem() const{
    return m_targetItem;
}

int EditTextUndoCommand::id() const{
    return 2;
}

// merge with previous edit undo command
bool EditTextUndoCommand::mergeWith(const QUndoCommand *command){
    // if not an editText command return
    if (command->id() != id() )
        return false;
    const EditTextUndoCommand *cmd = static_cast<const EditTextUndoCommand*>(command);
    // if this is a new edit text command but not for this item, return
    if ( cmd->sourceItem() != m_targetItem )
        return false;
    //update new value with value of this command of the new command and accept merging
    m_newTextItem->setItemName(cmd->textItem()->itemName());
    m_newTextItem->setBold(cmd->textItem()->bold());
    m_newTextItem->setItalic(cmd->textItem()->italic());
    m_newTextItem->setUnderlined(cmd->textItem()->underlined());
    m_newTextItem->setFont(cmd->textItem()->font());
    m_newTextItem->setWeight(cmd->textItem()->width());
    m_newTextItem->setText(cmd->textItem()->text());
    return true;
}
