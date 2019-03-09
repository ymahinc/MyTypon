#include "mytyponscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <qgraphicsview.h>
#include <QGraphicsRectItem>
#include "items/myitembase.h"
#include "global.h"
#include "tools/GroupTool/groupundocommand.h"
#include "tools/GroupTool/ungroupundocommand.h"
#include "tools/SelectTool/rotateundocommand.h"
#include "tools/SelectTool/mirrorundocommand.h"
#include "items/textitem/textitem.h"
#include "tools/SelectTool/changelayerundocommand.h"
#include "tools/SelectTool/alignongridundocommand.h"
#include "tools/TextTool/deletetextundocommand.h"
#include "items/paditem/padgroup.h"
#include "tools/PadTool/deletepadundocommand.h"
#include "tools/TrackTool/deletetrackundocommand.h"
#include "tools/SelectTool/alignangleongridundocommand.h"
#include "items/drawitem.h"
#include "tools/DrawTool/deletedrawundocommand.h"

MYTyponScene::MYTyponScene(QObject *parent) :
    QGraphicsScene(parent) {
    setMinimumRenderSize(5);
    m_typonItems = new QList<MYItemBase *>;
    m_selectedItems = new QList<MYItemBase *>;
    setItemIndexMethod(NoIndex);
    m_layersStack = new LayersStack(this);
}

MYTyponScene::~MYTyponScene(){
    delete m_typonItems;
    delete m_selectedItems;
}

QList<MYItemBase *> MYTyponScene::topMostItems(){
    QList<MYItemBase *> topMostItems;
    foreach(QGraphicsItem *item, items()){
        MYItemBase *it = dynamic_cast<MYItemBase *>(item);
        if ( it ){
            MYItemBase *grp = it->topMostGroup();
            if ( ! topMostItems.contains(grp) )
                topMostItems.append(grp);
        }
    }
    return topMostItems;
}

/*
MYItemBase *MYTyponScene::createGroup(QList<MYItemBase *> items, MYItemBase *group, bool temporary){
    // if group = 0 => We need a new group else use the one in parameter
    MYItemBase *grp;
    if ( group )
        grp = group;
    else
        grp = new MYItemBase(0,true);
    // if one of the group child was selected, group will be selected after creation
    bool atLeastOneChildWasSelected = false;
    // if not a temporary group, remove selected childs from selection list
    foreach(MYItemBase *it, items){
        if ( it->typonItemIsSelected() )
            atLeastOneChildWasSelected = true;
        grp->addToGroup(it);
        if ( !temporary)
            removeSelectedItem(it,false);
    }
    // set name and pudate group counter if group is not temporary
    if ( ! temporary && group == 0 )
        grp->setItemName(QObject::tr("Group %1").arg(++qApp->currentTypon()->groupCounter));
    // if not temporary grp is a new selected item
    if ( ! temporary && atLeastOneChildWasSelected )
        addSelectedItem(grp);
    return grp;
}*/

void MYTyponScene::createGroupFromSelection(){
    GroupUndoCommand *groupCommand = new GroupUndoCommand(*m_selectedItems);
    qApp->undoGroup()->activeStack()->push(groupCommand);
}

void MYTyponScene::deleteGroup(MYItemBase *group, bool updateSelection){
    if ( group->isGroup() && !group->isUngroupable() ){
        foreach (MYItemBase *child, group->childs()){
            if ( updateSelection ){
                // remove child group before updating selection (wich is based on the group of the item)
                child->setGroup(0);
                // if child is selectable and group was selected, select child otherwise unselect it
                if ( child->isEnabled() && child->isVisible() && group->typonItemIsSelected() )
                    addSelectedItem(child,false);
                else
                    removeSelectedItem(child,false);
            }
            // actually remove child from group
            group->removeFromGroup(child);
        }
        if ( updateSelection ){
            // remove group from selected items as it does not exists for the scene anymore
            // object is always valid and will be delete in the undo command wich created it
            m_selectedItems->removeOne(group);
            emit typonItemsSelectionChanged();
        }
    }
}

void MYTyponScene::deleteGroupsFromSelection(){
    bool macroStarted = false;
    foreach(MYItemBase *it, *m_selectedItems){
        if ( it->isGroup() && !it->isUngroupable() ){
            if ( !macroStarted ){
                macroStarted = true;
                qApp->undoGroup()->activeStack()->beginMacro(tr("Ungroup"));
            }

            UngroupUndoCommand *ungroupCommand = new UngroupUndoCommand(it);
            qApp->undoGroup()->activeStack()->push(ungroupCommand);
        }
    }
    if ( macroStarted )
        qApp->undoGroup()->activeStack()->endMacro();
}

void MYTyponScene::selectAll(){
    m_selectedItems->clear();
    foreach (QGraphicsItem *it, items()){
        MYItemBase *itB = dynamic_cast<MYItemBase *>(it);
        if ( itB ){
            if ( itB->isVisible() && itB->isEnabled() )
                addSelectedItem(itB->topMostGroup(),false);
        }
    }
    emit typonItemsSelectionChanged();
}

LayersStack *MYTyponScene::layers(){
    return m_layersStack;
}

// add a typon item to the list, add it to the desired layer
void MYTyponScene::addTyponItem(MYItemBase *typonItem, bool onlyUpdateLists){
    if ( ! onlyUpdateLists )
        addItem(typonItem);

    m_typonItems->append(typonItem);
}

// remove a typon item
void MYTyponScene::removeTyponItem(MYItemBase *typonItem, bool onlyUpdateLists){
    // if item was selected update delected items list
    if ( typonItem->typonItemIsSelected() )
        removeSelectedItem(typonItem);

    m_typonItems->removeOne(typonItem);
    if ( ! onlyUpdateLists )
        removeItem(typonItem);
}

QList<MYItemBase *> *MYTyponScene::typonItems(){
    return m_typonItems;
}

// find if at least one item is visible in the scene
bool MYTyponScene::atLeastOneTyponItemIsVisible(){
    bool oneOrMoreItemIsVisible = false;
    for(int i = 0; i < m_layersStack->count(); i++){
        if ( m_layersStack->at(i)->isVisible() && m_layersStack->at(i)->childItems().count() ){
            oneOrMoreItemIsVisible = true;
            break;
        }
    }
    return oneOrMoreItemIsVisible;
}

// calculate the items bounding rect
QRectF MYTyponScene::typonItemsBoundingRect(){
    QRectF bRect = QRectF();
    for(int i = 0; i < m_layersStack->count(); i++)
        bRect = bRect.united(m_layersStack->at(i)->boundingRect());
    return bRect;
}

// recalculate ground items shape colliding with the given rect
// used when an item is added or moved in the scene
void MYTyponScene::updateGroundPlanes(QRectF rect, Layer *layer){
    // if layer is specified, only update this layer ground planes, else update all ground planes
    if ( layer ){
        layer->updateGroundPlanes(rect);
    }else{
        for(int i = 0; i < m_layersStack->count(); i++)
            m_layersStack->at(i)->updateGroundPlanes(rect);
    }
}

// deselect all selected items
void MYTyponScene::clearSelection(){
    bool atLeastOneSelected = m_selectedItems->count();
    for(int i =0; i < m_selectedItems->count(); i++ )
        m_selectedItems->at(i)->setTyponItemSelected(false);
    m_selectedItems->clear();
    // if one item was selected before clear then inform that selection has changed
    if ( atLeastOneSelected )
        emit typonItemsSelectionChanged();
}

// add new item to selected list
void MYTyponScene::addSelectedItem(MYItemBase *item, bool emitSignal){
    if ( item->isAngle() )
        return;
    MYItemBase *topMostItem = item->topMostGroup();
    if ( m_selectedItems->contains(topMostItem) )
        return;
    topMostItem->setTyponItemSelected(true);
    m_selectedItems->append(topMostItem);
    if ( emitSignal )
        emit typonItemsSelectionChanged();
}

// remove itemfrom selected list
void MYTyponScene::removeSelectedItem(MYItemBase *item, bool emitSignal){
    if ( ! m_selectedItems->contains(item) )
        return;
    item->setTyponItemSelected(false);
    m_selectedItems->removeOne(item);
    if ( emitSignal )
        emit typonItemsSelectionChanged();
}

// get bounding rect for all selected items
QRectF MYTyponScene::selectedItemsBoundingRect(){
    /*QRectF bRect;
    for(int i =0; i < m_selectedItems->count(); i++ )
        bRect = bRect.united(m_selectedItems->at(i)->sceneBoundingRect());*/

    QRectF bRect;
    foreach(MYItemBase* item, *m_selectedItems)
        bRect = bRect.united(item->groupBoundingRect());
    return bRect;
}

QList<MYItemBase *> *MYTyponScene::selectedTyponItems(){
    return m_selectedItems;
}

// get all typon items for a given rect (used by selectTool when rubberBanding)
QList<MYItemBase *> MYTyponScene::typonItemsInRect(QRectF rect){
    QList<MYItemBase *> itemsInRect;
    QList<QGraphicsItem *>  its = items(rect);
    foreach (QGraphicsItem *it, its){
        MYItemBase *itB = dynamic_cast<MYItemBase *>(it);
        if ( itB )
                itemsInRect.append(itB);
    }
    return itemsInRect;
}

// rotate a list of items by "angle" degrees
void MYTyponScene::rotateTyponItems(QList<MYItemBase *> items, double angle){
    RotateUndoCommand *rotateCommand = new RotateUndoCommand(items,angle);
    qApp->undoGroup()->activeStack()->push(rotateCommand);
}

void MYTyponScene::rotateSelection(double angle){
    rotateTyponItems(*m_selectedItems,angle);
}

void MYTyponScene::rotateSelectionLeft(){
    rotateTyponItems(*m_selectedItems,-45);
}

void MYTyponScene::rotateSelectionRight(){
    rotateTyponItems(*m_selectedItems,45);
}

// horizontal mirror for items in provided list
void MYTyponScene::horizontalMirror(QList<MYItemBase *> items){
    MirrorUndoCommand *mirrorCommand = new MirrorUndoCommand(items,true);
    qApp->undoGroup()->activeStack()->push(mirrorCommand);
}

// vertical mirror for items in provided list
void MYTyponScene::verticalMirror(QList<MYItemBase *> items){
    MirrorUndoCommand *mirrorCommand = new MirrorUndoCommand(items,false);
    qApp->undoGroup()->activeStack()->push(mirrorCommand);
}

void MYTyponScene::horizontalMirrorSelection(){
    horizontalMirror(*m_selectedItems);
}

void MYTyponScene::verticalMirrorSelection(){
    verticalMirror(*m_selectedItems);
}

// align selected items on grid
// an undo macro is started as they could be many items to align on grid
void MYTyponScene::alignSelectedOnGrid(){
    bool macroStarted = false;

    foreach(MYItemBase* item, *m_selectedItems){
        // find if selected items contains alignable items
        TextItem *txtItem = dynamic_cast<TextItem *>(item->topMostGroup());
        MYItemBase *baseItem = dynamic_cast<MYItemBase *>(item->topMostGroup());
        Track *trackItem = dynamic_cast<Track *>(item->topMostGroup());
        if ( baseItem ){
            // if item is a group or a text item
            if ( baseItem->isGroup() || txtItem ){
                // if macro is not started yet, lets start it
                if ( !macroStarted ){
                    macroStarted = true;
                    qApp->undoGroup()->activeStack()->beginMacro(tr("Align"));
                }
                // align this item on grid
                alignItem(item);
            }
            if ( trackItem ){
                // if macro is not started yet, lets start it
                if ( !macroStarted ){
                    macroStarted = true;
                    qApp->undoGroup()->activeStack()->beginMacro(tr("Align"));
                }
                foreach (Angle *angle, trackItem->angles()){
                    AlignAngleOnGridUndoCommand *alignAngleCommand = new AlignAngleOnGridUndoCommand(angle,
                                                    qApp->currentTypon()->typonView()->nearestGridPoint(angle->groupPos()));
                    qApp->undoGroup()->activeStack()->push(alignAngleCommand);
                }
            }
        }
    }

    // if available items for align were found, then a macro has been started, we need to end it
    if ( macroStarted )
        qApp->undoGroup()->activeStack()->endMacro();
}

// actually align items on grid
void MYTyponScene::alignItem(MYItemBase *item){
    AlignOnGridUndoCommand *alignCommand = new AlignOnGridUndoCommand(item);
    qApp->undoGroup()->activeStack()->push(alignCommand);
}

// send selected items to desired layer
// an undo macro is started as they could be many items to update
void MYTyponScene::sendSelectedToLayer(Layer *layer){
    bool macroStarted = false;
    // find if selected items contains items for wich layer can be changed
    foreach(MYItemBase* item, *m_selectedItems){
        TextItem *txtItem = dynamic_cast<TextItem *>(item->topMostGroup());
        Track *trackItem = dynamic_cast<Track *>(item->topMostGroup());
        MYItemBase *baseItem = dynamic_cast<MYItemBase *>(item->topMostGroup());
        if ( baseItem ){
            // if item is a group or a text item
            if ( baseItem->isGroup() || txtItem || trackItem ){
                // if macro is not started yet, lets start it
                if ( !macroStarted ){
                    macroStarted = true;
                    qApp->undoGroup()->activeStack()->beginMacro(tr("Send to layer"));
                }
                // actually modify item layer
                sentItemToLayer(baseItem,layer);
            }
        }
    }
    // if available items for layer change were found, then a macro has been started, we need to end it
    if ( macroStarted )
        qApp->undoGroup()->activeStack()->endMacro();
}

// change layer for the item "item"
void MYTyponScene::sentItemToLayer(MYItemBase *item, Layer *layer){
    if ( item->isGroup() ){
        // if item is a group, it could be a pad
        PadGroup *pad = dynamic_cast<PadGroup *>(item);
        if ( pad ){
            // if it is a pad, treat it as a single item
            ChangeLayerUndoCommand *changeLayerCommand = new ChangeLayerUndoCommand(item,layer);
            qApp->undoGroup()->activeStack()->push(changeLayerCommand);
        }else{
            // item is only a basic group, send all its children to desired layer recursively
            foreach(MYItemBase* it, item->childs() )
                sentItemToLayer(it,layer);
        }
    }else{
        // if item is a simple item, update its layer
        ChangeLayerUndoCommand *changeLayerCommand = new ChangeLayerUndoCommand(item,layer);
        qApp->undoGroup()->activeStack()->push(changeLayerCommand);
    }
}

// delete all selected items, if function was called by copy/paste/cut action, set undo command name accordingly
void MYTyponScene::deleteSelection(bool cut){
    // cretae an undo macro as many items could be deleted at once
    if ( cut )
        qApp->undoGroup()->activeStack()->beginMacro(tr("Cut"));
    else
        qApp->undoGroup()->activeStack()->beginMacro(tr("Delete"));

    foreach(MYItemBase* item, *m_selectedItems)
        deleteItem(item); // actually delete items

    qApp->undoGroup()->activeStack()->endMacro();
}

// delete ( in fact no actual delete, items are only removed from scene) item
void MYTyponScene::deleteItem(MYItemBase *item){
    // if item is a group
    if ( item->isGroup() ){
        // remove group from selection list
        removeSelectedItem(item);        
        // test if this group is a pad item
        PadGroup *padItem = dynamic_cast<PadGroup *>(item);
        if ( padItem ){
            // the item is a paditem, call its remove undo command
            DeletePadUndoCommand *deletePadCommand = new DeletePadUndoCommand(padItem);
            qApp->undoGroup()->activeStack()->push(deletePadCommand);
        }else{
            // item is only a classic group, remove its children recursivley
            foreach (MYItemBase *it, item->childs() )
                deleteItem(it);
        }
    }else{
        // If item is a TextItem, remove it via its remove undo command
        TextItem *txtItem = dynamic_cast<TextItem *>(item);
        if ( txtItem ){
            DeleteTextUndoCommand *deleteTextCommand = new DeleteTextUndoCommand(txtItem);
            qApp->undoGroup()->activeStack()->push(deleteTextCommand);
            return;
        }
        // If item is a Drawing, remove it via its remove undo command
        DrawItem *drawItem = dynamic_cast<DrawItem *>(item);
        if ( drawItem ){
            DeleteDrawUndoCommand *deleteDrawCommand = new DeleteDrawUndoCommand(drawItem);
            qApp->undoGroup()->activeStack()->push(deleteDrawCommand);
            return;
        }
        // If item is a Track, remove it via its remove undo command
        Track *trackItem = dynamic_cast<Track *>(item);
        if ( trackItem ){
            DeleteTrackUndoCommand *deleteTrackCommand = new DeleteTrackUndoCommand(trackItem);
            qApp->undoGroup()->activeStack()->push(deleteTrackCommand);
            return;
        }
    }
}

void MYTyponScene::test(){
    QRectF rect;
    foreach(MYItemBase* item, *m_selectedItems)
        rect = rect.united(item->groupBoundingRect());

    QGraphicsRectItem *r = new QGraphicsRectItem(rect);
    r->setZValue(100000);
    QPen p;
    p.setColor(Qt::yellow);
    p.setCosmetic(true);
    r->setPen(p);
    addItem(r);

}
