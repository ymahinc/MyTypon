#ifndef MYTYPONSCENE_H
#define MYTYPONSCENE_H

// scene used by typon view
// get list of typonitems
// manage custom list for selected items
// have a stack of layers

#include <QGraphicsScene>
#include <QPainter>
#include <QApplication>
#include "layers/layersstack.h"
#include "items/myitembase.h"

class MYTyponScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit MYTyponScene(QObject *parent = 0);
    ~MYTyponScene();

    LayersStack *layers();
    void addTyponItem(MYItemBase *typonItem, bool onlyUpdateLists = false);
    void removeTyponItem(MYItemBase *typonItem, bool onlyUpdateLists = false);
    QList<MYItemBase *> *typonItems();
    //MYItemBase *createGroup(QList<MYItemBase *> items, MYItemBase *group = 0, bool temporary = false);
    void deleteGroup(MYItemBase *group, bool updateSelection = true);
    bool atLeastOneTyponItemIsVisible();
    QRectF typonItemsBoundingRect();
    void updateGroundPlanes(QRectF rect, Layer *layer = 0);
    void addSelectedItem(MYItemBase *item, bool emitSignal = true);
    void removeSelectedItem(MYItemBase *item, bool emitSignal = true);
    QRectF selectedItemsBoundingRect();
    QList<MYItemBase *> *selectedTyponItems();
    QList<MYItemBase *> typonItemsInRect(QRectF rect);
    QList<MYItemBase *> topMostItems();

public slots:
    void createGroupFromSelection();
    void rotateSelectionLeft();
    void rotateSelectionRight();
    void rotateSelection(double angle);
    void horizontalMirrorSelection();
    void verticalMirrorSelection();
    void deleteGroupsFromSelection();
    void selectAll();
    void clearSelection();
    void alignSelectedOnGrid();
    void sendSelectedToLayer(Layer *layer);
    void deleteSelection(bool cut = false);
    void test();

signals:
    void typonItemsSelectionChanged();

private:    
    void rotateTyponItems(QList<MYItemBase *> items, double angle);
    void horizontalMirror(QList<MYItemBase *> items);
    void verticalMirror(QList<MYItemBase *> items);
    LayersStack *m_layersStack;
    QList<MYItemBase *> *m_typonItems;
    QList<MYItemBase *> *m_selectedItems;
    void sentItemToLayer(MYItemBase *item, Layer *layer);
    void alignItem(MYItemBase *item);
    void deleteItem(MYItemBase *item);
};

#endif // MYTYPONSCENE_H
