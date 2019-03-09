#include "selecttool.h"
#include "ui_selecttool.h"
#include "global.h"
#include <QDebug>
#include "items/textitem/textitem.h"
#include "layers/layer.h"
#include "items/paditem/paditem.h"
#include "items/paditem/holeitem.h"
#include "items/trackitem/track.h"
#include "items/trackitem/angle.h"
#include "items/drawitem.h"
#include "removeangleundocommand.h"
#include "alignangleongridundocommand.h"
#include "splittrackundocommand.h"
#include "mergetrackundocommand.h"
#include "moveangleundocommand.h"
#include "insertangleundocommand.h"

SelectTool::SelectTool(QWidget *parent, QMenu *contextMenu)
   : MYToolBase(parent), ui(new Ui::SelectTool){
    ui->setupUi(this);
    qApp->setSelectionTree(ui->treeWidget);
    setAcceptRubberband(true);
    setAcceptAutoExpand(true);
    setToolName(tr("Select"));
    setToolIcon(QIcon(":/pictures/select.png"));
    m_cursor = Qt::ArrowCursor;
    m_contextMenu = contextMenu;
    m_isReadyToDragItems = false;
    m_isReadyToDragAngle = false;
    m_draggingAngle = 0;
}

SelectTool::~SelectTool(){
    delete ui;
}

MYItemBase *SelectTool::typonItemAt(QPointF pos, QTransform transform){
    MYItemBase *itB = 0;
    foreach( QGraphicsItem *it, qApp->currentTypon()->typonScene()->items(pos,Qt::IntersectsItemShape,Qt::DescendingOrder,transform) ){
        Layer *lay = dynamic_cast<Layer *>(it);
        if ( !lay ){
            itB = dynamic_cast<MYItemBase *>(it);
            break;
        }
    }
    return itB;
}

void SelectTool::handleMousePressEvent(QMouseEvent *event){
    if ( event->button() == Qt::MidButton )
        MYToolBase::mousePressEventTool(event);
    // left mouse button is clicked
    if ( event->button() == Qt::LeftButton ) {
        QPointF scenePos = qApp->currentTypon()->typonView()->mapToScene(event->pos());

        //MYItemBase *it = dynamic_cast<MYItemBase *>(typonItemAt(scenePos,QTransform()));
        MYItemBase *it = typonItemAt(scenePos,QTransform());
        Angle *angleIt = dynamic_cast<Angle *>(it);

        // if there is an item under mouse cursor select/deselect it
        if ( it ){
            // ready to move item
            m_isReadyToDragItems = true;

            if (angleIt){
                m_isReadyToDragAngle = true;
                m_draggingAngle = angleIt;
                m_oldAnglePos = angleIt->groupPos();
            }
            //m_lasPos = qApp->currentTypon()->typonView()->mapToScene(event->pos());
            m_lastPos = qApp->currentTypon()->typonView()->nearestGridPoint(
                    qApp->currentTypon()->typonView()->mapToScene(event->pos()));

            // delta from origin point when moving
            m_totalDelta = QPointF(0,0);

            QPointF scenePos = qApp->currentTypon()->typonView()->mapToScene(event->pos());
            //MYItemBase *it = dynamic_cast<MYItemBase *>(typonItemAt(scenePos,QTransform()));
            MYItemBase *it = typonItemAt(scenePos,QTransform());

            // if item is selectable, toggle item selection
            if ( m_ctrlKeyIsPressed ){
                if ( it->isEnabled() && it->isVisible() ){
                    MYItemBase *topMostItem = it->topMostGroup();
                    if ( topMostItem->typonItemIsSelected() ){
                        qApp->currentTypon()->typonScene()->removeSelectedItem(topMostItem);
                    }else{
                        qApp->currentTypon()->typonScene()->addSelectedItem(topMostItem);
                    }
                }
            }else{
                if ( it->isEnabled() && it->isVisible() ){
                    MYItemBase *topMostItem = it->topMostGroup();
                    if ( !topMostItem->typonItemIsSelected() ){
                        qApp->currentTypon()->typonScene()->clearSelection();
                        qApp->currentTypon()->typonScene()->addSelectedItem(topMostItem);
                    }
                }
            }
        }else{
            // if no item under mouse
            // if not ctrl key then clear selection
            if ( ! m_ctrlKeyIsPressed )
                qApp->currentTypon()->typonScene()->clearSelection();
            // let bas tool handling rubberbanding
            MYToolBase::mousePressEventTool(event);
        }
        event->accept();
    }
    // right clic
    if ( event->button() == Qt::RightButton ) {
        QPointF scenePos = qApp->currentTypon()->typonView()->mapToScene(event->pos());
        MYItemBase *it = typonItemAt(scenePos,QTransform());
        // if item under mouse, select it and provide a context menu
        if ( it ){
            if ( ! it->typonItemIsSelected() )
                qApp->currentTypon()->typonScene()->addSelectedItem(it);
            TextItem *textIt = dynamic_cast<TextItem *>(it);
            PadItem *padIt = dynamic_cast<PadItem *>(it);
            HoleItem *holeIt = dynamic_cast<HoleItem *>(it);
            Track *trackIt = dynamic_cast<Track *>(it);
            Angle *angleIt = dynamic_cast<Angle *>(it);

            if ( angleIt ){
                m_selectedAngle = angleIt;

                Track *track = dynamic_cast<Track *>(angleIt->parentItem());
                if ( !track )
                    return;

                QMenu angleMenu;
                QAction removeAngle;
                removeAngle.setText(tr("Remove angle"));
                QAction alignAngle;
                alignAngle.setText(tr("Align angle on grid"));
                QAction alignAllAngles;
                alignAllAngles.setText(tr("Align all angles on grid"));
                QAction divide;
                divide.setText(tr("Divide track"));
                QAction merge;
                merge.setText(tr("Merge tracks"));

                if ( track->angles().count() <= 2 )
                    removeAngle.setEnabled(false);

                if ( track->angles().count() < 3 || angleIt == track->angles().at(0)
                     || angleIt == track->angles().last() ){
                    divide.setEnabled(false);
                }

                if ( !track->angleIsOnExtremity(angleIt) )
                    merge.setEnabled(false);

                DrawItem *drawItem = dynamic_cast<DrawItem *>(angleIt->parentItem());
                if ( drawItem ){
                    if ( drawItem->drawType() == DrawItem::Rectangle || drawItem->drawType() == DrawItem::Ellipse){
                        removeAngle.setEnabled(false);
                    }
                    divide.setEnabled(false);
                    merge.setEnabled(false);
                }

                angleMenu.addAction(&removeAngle);
                connect(&removeAngle,SIGNAL(triggered(bool)),this,SLOT(removeAngle()));
                angleMenu.addAction(&alignAngle);
                connect(&alignAngle,SIGNAL(triggered(bool)),this,SLOT(alignSelectedAngle()));
                angleMenu.addAction(&alignAllAngles);
                connect(&alignAllAngles,SIGNAL(triggered(bool)),this,SLOT(alignAngles()));
                angleMenu.addSeparator();
                angleMenu.addAction(&divide);
                connect(&divide,SIGNAL(triggered(bool)),this,SLOT(splitTrack()));
                angleMenu.addAction(&merge);
                connect(&merge,SIGNAL(triggered(bool)),this,SLOT(mergeTracks()));
                angleMenu.exec(event->globalPos());
            }
            if ( trackIt ){
                if ( ! trackIt->group() ){
                    m_selectedTrack = trackIt;
                    m_insertPos = event->pos();
                    bool addInsertAction = true;
                    DrawItem *drawItem = dynamic_cast<DrawItem *>(trackIt);
                    if ( drawItem ){
                        if ( drawItem->drawType() == DrawItem::Rectangle || drawItem->drawType() == DrawItem::Ellipse)
                            addInsertAction = false;
                        if ( drawItem->drawType() == DrawItem::Polygon
                             && drawItem->indexOfPoint(qApp->currentTypon()->typonView()->mapToScene(m_insertPos)) == -1 )
                            addInsertAction = false;
                    }
                    QAction insertAngle;
                    if ( addInsertAction ){
                        insertAngle.setText(tr("Insert angle"));
                        connect(&insertAngle,SIGNAL(triggered(bool)),this,SLOT(insertAngle()));
                        m_contextMenu->insertAction(m_contextMenu->actions().at(0),&insertAngle);
                        m_contextMenu->insertSeparator(m_contextMenu->actions().at(1));
                    }
                    m_contextMenu->exec(event->globalPos());
                }else{
                    m_contextMenu->exec(event->globalPos());
                }
            }
            if ( textIt || padIt || holeIt ){
                m_contextMenu->exec(event->globalPos());   
            }
        }else{
            QMenu defautMenu;
            QAction showOptions;
            showOptions.setText(tr("Typon options..."));
            connect(&showOptions,SIGNAL(triggered(bool)),this,SIGNAL(showOptionsSignal()));
            defautMenu.addAction(&showOptions);
            defautMenu.exec(event->globalPos());
        }
    }
}

void SelectTool::insertAngle(){
    QPointF newPos = qApp->currentTypon()->typonView()->mapToScene(m_insertPos);
    InsertAngleUndoCommand *insertAngleCommand = new InsertAngleUndoCommand(m_selectedTrack,newPos);
    qApp->undoGroup()->activeStack()->push(insertAngleCommand);
}

void SelectTool::mergeTracks(){
    Track *track = dynamic_cast<Track *>(m_selectedAngle->parentItem());
    if ( !track )
        return;
    bool trackToMergeFound = false;
    foreach (QGraphicsItem *it, qApp->currentTypon()->typonScene()->items(m_selectedAngle->groupPos())) {
        Angle *angle = dynamic_cast<Angle *>(it);
        if (angle){
            if ( angle != m_selectedAngle ){
                DrawItem *draw = dynamic_cast<DrawItem *>(angle->parentItem());
                Track *track2 = dynamic_cast<Track *>(angle->parentItem());
                if ( track2 && ! draw ){
                    if ( track != track2 ){
                        if ( track2->angleIsOnExtremity(angle) ){
                            trackToMergeFound = true;
                            MergeTrackUndoCommand *mergeCommand = new MergeTrackUndoCommand(m_selectedAngle,angle);
                            qApp->undoGroup()->activeStack()->push(mergeCommand);
                        }
                    }
                }
            }
        }
    }
    if ( !trackToMergeFound )
        qApp->statusBar()->showMessage(QObject::tr("/!\\ No track found to merge with /!\\"),2000);
}

void SelectTool::splitTrack(){
    SplitTrackUndoCommand *splitCommand = new SplitTrackUndoCommand(m_selectedAngle);
    qApp->undoGroup()->activeStack()->push(splitCommand);
}

void SelectTool::aligneAngle(Angle *angle){
    Track *track = dynamic_cast<Track *>(angle->parentItem());
    if ( !track )
        return;
    QPointF newPos = qApp->currentTypon()->typonView()->nearestGridPoint(angle->groupPos());
    if ( angle->groupPos() == newPos )
        return;
    AlignAngleOnGridUndoCommand *alignAngleCommand = new AlignAngleOnGridUndoCommand(angle,newPos);
    qApp->undoGroup()->activeStack()->push(alignAngleCommand);
}

void SelectTool::alignSelectedAngle(){
    Track *track = dynamic_cast<Track *>(m_selectedAngle->parentItem());
    if ( !track )
        return;
    aligneAngle(m_selectedAngle);
}

void SelectTool::alignAngles(){
    Track *track = dynamic_cast<Track *>(m_selectedAngle->parentItem());
    if ( !track )
        return;
    qApp->undoGroup()->activeStack()->beginMacro(QObject::tr("Align Angle"));
    foreach (Angle *angle, track->angles())
        aligneAngle(angle);
    qApp->undoGroup()->activeStack()->endMacro();
}

void SelectTool::removeAngle(){
    RemoveAngleUndoCommand *removeAngleCommand = new RemoveAngleUndoCommand(m_selectedAngle);
    qApp->undoGroup()->activeStack()->push(removeAngleCommand);
}

void SelectTool::handleMouseReleaseEvent(QMouseEvent *event){
    if ( event->button() == Qt::MidButton )
        MYToolBase::mouseReleaseEventTool(event);
    // we are not rubberbanding anymore, clear list of items present in rubberband
    if ( event->button() == Qt::LeftButton ) {
        // if we were moving items around the scene, actually move it
        if ( m_isReadyToDragItems && !m_totalDelta.isNull() ){
            if ( m_isReadyToDragAngle ){
                MoveAngleUndoCommand *moveAngleCommand = new MoveAngleUndoCommand(m_draggingAngle,m_oldAnglePos,m_draggingAngle->scenePos());
                qApp->undoGroup()->activeStack()->push(moveAngleCommand);
            }else{
                MoveUndoCommand *moveCommand = new MoveUndoCommand(*qApp->currentTypon()->typonScene()->selectedTyponItems(),m_totalDelta);
                qApp->undoGroup()->activeStack()->push(moveCommand);
            }
        }
        m_isReadyToDragItems = false;
        m_isReadyToDragAngle = false;
        m_draggingAngle = 0;
        if ( m_isRubberBanding )
            m_itemsInBoundingRect.clear();
        MYToolBase::mouseReleaseEventTool(event);
        event->accept();
    }
}

void SelectTool::handleMouseMoveEvent(QMouseEvent *event){
    MYToolBase::mouseMoveEventTool(event);
    // if we are rubberbanding
    if ( m_isRubberBanding ){
        // get a copy of items present in previous rubberband rect so we can unselect not present ones in the new rect
        QList<MYItemBase *> unselectItems = m_itemsInBoundingRect;
        // clear old items in rubberband rect list
        m_itemsInBoundingRect.clear();
        // get a new list of all items in rubber band rect
        QList<MYItemBase *> newItems = qApp->currentTypon()->typonScene()->typonItemsInRect(m_rubberBandRect);
        foreach (MYItemBase *it, newItems) {
            if ( it->isEnabled() && it->isVisible() ){
                MYItemBase *topMostItem = it->topMostGroup();
                // add this item in the list of items present in rubberband rect
                if ( !m_itemsInBoundingRect.contains(topMostItem) ){
                    m_itemsInBoundingRect.append(topMostItem);
                    qApp->currentTypon()->typonScene()->addSelectedItem(topMostItem);
                }
                // remove it from the unselected list as it is present in the new selection
                if ( unselectItems.contains(topMostItem) )
                    unselectItems.removeOne(topMostItem);
            }
        }

        // remove all remaining items in unselected list from selection
        foreach (MYItemBase *it, unselectItems)
            qApp->currentTypon()->typonScene()->removeSelectedItem(it->topMostGroup());
    }
    // if we are moving items
    if ( m_isReadyToDragItems ){
        // cmopute new position
        QPointF newPoint = qApp->currentTypon()->typonView()->nearestGridPoint(
                    qApp->currentTypon()->typonView()->mapToScene(event->pos()));
        // get current delta from last mouse pos
        QPointF clicDelta = newPoint - m_lastPos;
        // update last mouse pos
        m_lastPos = newPoint;
        // cumulate clicDelta with total wich is the delta from start moving point
        m_totalDelta += clicDelta;

        if ( m_isReadyToDragAngle){
            Track *track = dynamic_cast<Track *>(m_draggingAngle->parentItem());
            if ( !track )
                return;
            QPointF newPos = m_draggingAngle->scenePos()+clicDelta;
            track->updateAnglePos(m_draggingAngle,newPos);
        }else{
            // move item by delta
            foreach (MYItemBase *it, *qApp->currentTypon()->typonScene()->selectedTyponItems() )
                it->moveItemBy(clicDelta);
        }

        // auto expand if out view port
        if ( !qApp->currentTypon()->typonView()->viewport()->rect().contains(event->pos()) )
            startExtending();
        else
            stopExtending();
    }
}

void SelectTool::handleWheelEvent(QWheelEvent *event){
    MYToolBase::mouseWheelEventTool(event);
}

void SelectTool::handleKeyPressEvent(QKeyEvent *event){
    MYToolBase::keyPressEventTool(event);
}

void SelectTool::handleKeyReleaseEvent(QKeyEvent *event){
    MYToolBase::keyReleaseEventTool(event);
}

void SelectTool::handleEnterEvent(QEvent *event){
    MYToolBase::enterEventTool(event);
}

void SelectTool::handleLeaveEvent(QEvent *event){
    MYToolBase::leaveEventTool(event);
}

void SelectTool::updateList(){
    ui->treeWidget->updateList();
}
