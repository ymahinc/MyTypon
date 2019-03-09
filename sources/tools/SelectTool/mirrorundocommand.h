#ifndef MIRRORUNDOCOMMAND_H
#define MIRRORUNDOCOMMAND_H

#include <QUndoCommand>
#include "items/myitembase.h"

class MirrorUndoCommand : public QUndoCommand
{
public:
    MirrorUndoCommand(QList<MYItemBase *> items, bool horizontal);
    void undo();
    void redo();

private:
    bool m_horizontal;
    QList<MYItemBase *> m_items;
    QRectF itemsBoundingRect();
};

#endif // MIRRORUNDOCOMMAND_H
