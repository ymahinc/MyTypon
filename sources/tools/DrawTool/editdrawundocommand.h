#ifndef EDITDRAWUNDOCOMMAND_H
#define EDITDRAWUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/drawitem.h"

class EditDrawUndoCommand : public QUndoCommand
{
public:
    EditDrawUndoCommand(DrawItem *item, QString itemName, double width, DrawItem::FillType fillType, double groundPlaneOffset);
    ~EditDrawUndoCommand();
    void undo();
    void redo();
    int id() const;
    bool mergeWith(const QUndoCommand *command);
    DrawItem *drawItem() const;
    DrawItem *sourceItem() const;

private:
    DrawItem m_oldDrawItem;
    DrawItem *m_newDrawItem;
    DrawItem *m_targetItem;
};

#endif // EDITDRAWUNDOCOMMAND_H
