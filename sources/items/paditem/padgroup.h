#ifndef PADGROUP_H
#define PADGROUP_H

/*
 * item that handle pad
 * It is a custom group wich handles a pad (copper part of the pad)
 * and a hole (which representate the drill hole)
 */

#include "paditem.h"
#include "holeitem.h"
#include "layers/layer.h"
#include "../myitembase.h"
class PadTool;

class PadGroup : public MYItemBase
{
public:
    PadGroup(MYItemBase *parent = 0, bool incrementCounter = false);
    ~PadGroup();
    PadGroup *clone();
    PadItem *pad();
    HoleItem *hole();
    void setLayer(Layer *layer);
    void setHasHole(bool hasHole);
    bool hasHole();
    void setHoleWidth(double width);
    void setType(PadItem::PadType type);
    void setVia(bool via);
    bool isVia();
    void setTool(PadTool *tool);
    PadTool *tool();
    void save(QDataStream &stream);
    int restore(QDataStream &stream);

private:
    PadItem *m_pad;
    HoleItem *m_hole;
    bool m_hasHole;
    Layer *m_layer;
    bool m_isVia;
    PadTool *m_padTool;
    //QString m_name;
};

#endif // PADGROUP_H
