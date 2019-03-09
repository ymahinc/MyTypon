#ifndef EDITTEXTUNDOCOMMAND_H
#define EDITTEXTUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/textitem/textitem.h"

class EditTextUndoCommand : public QUndoCommand
{
public:
    EditTextUndoCommand(TextItem *item, QString itemName,
                        QString itemText, QFont font, double weight);
    ~EditTextUndoCommand();
    void undo();
    void redo();
    int id() const;
    bool mergeWith(const QUndoCommand *command);
    TextItem *textItem() const;
    TextItem *sourceItem() const;

private:
    TextItem m_oldTextItem;
    TextItem *m_newTextItem;
    TextItem *m_targetItem;
};

#endif // EDITTEXTUNDOCOMMAND_H
