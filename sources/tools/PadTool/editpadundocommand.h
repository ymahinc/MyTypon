#ifndef EDITPADUNDOCOMMAND_H
#define EDITPADUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/paditem/padgroup.h"
#include "items/paditem/paditem.h"

class EditPadUndoCommand : public QUndoCommand
{
public:
    EditPadUndoCommand(PadGroup *item, QString itemName, PadItem::PadType padType, double width,
                       bool hasHole, double holeWidth, bool isVia);
    ~EditPadUndoCommand();
    void undo();
    void redo();
    int id() const;
    bool mergeWith(const QUndoCommand *command);
    PadGroup *padGroup() const;
    PadGroup *sourceItem() const;

private:
    PadGroup m_oldPadItem;
    PadGroup *m_newPadItem;
    PadGroup *m_targetItem;
};

#endif // EDITPADUNDOCOMMAND_H
