#include "mytyponruler.h"
#include <QScrollBar>
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QStaticText>
#include "global.h"

MYTyponRuler::MYTyponRuler(MYTyponScene *typonScene, QWidget *parent, Orientations orientation)
   : QGraphicsView(typonScene, parent), m_orientation(orientation){
    init();
}

// init the ruler view, size depending of the orientation
void MYTyponRuler::init(){
    m_lasMousePos = QPointF();
    m_linesVisible = false;

    setViewportUpdateMode(MinimalViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing,true);

    m_scene = new QGraphicsScene(this);

    //set width or height at 50000: it is the size of the typon scene
    if ( m_orientation == Horizontal ) {
        m_width = 50000;
        m_height = 21;
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }else{
        m_width = 21;
        m_height = 50000;
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    m_scene->setSceneRect(0, 0, m_width, m_height);

    setScene(m_scene);
    verticalScrollBar()->setEnabled(false);
    horizontalScrollBar()->setEnabled(false);

    connect(qApp->workSpace(),SIGNAL(workspaceUnitChanged()),this,SLOT(refresh()));
}

// called by external function to inform ruler that mouse position has changed in the view
void MYTyponRuler::updatePos(QPointF pos){
    m_linesVisible = true;
    // get viewport rect in scene coordinates
    QRectF viewPortSceneRect = mapToScene(viewport()->rect()).boundingRect();

    // redraw lines in old mouse position
    if ( m_orientation == Vertical )
        m_scene->invalidate(QRectF(viewPortSceneRect.left(),m_lasMousePos.y(),viewPortSceneRect.width(),1.0).adjusted(-0.5,-0.5,0.5,0.5),QGraphicsScene::AllLayers);
    else
        m_scene->invalidate(QRectF(m_lasMousePos.x(),viewPortSceneRect.top(),1.0,viewPortSceneRect.height()).adjusted(-0.5,-0.5,0.5,0.5),QGraphicsScene::AllLayers);
    //update mouse position with new data and update the lines again
    m_lasMousePos = pos;
    if ( m_orientation == Vertical )
        m_scene->invalidate(QRectF(viewPortSceneRect.left(),m_lasMousePos.y(),viewPortSceneRect.width(),1.0).adjusted(-0.5,-0.5,0.5,0.5),QGraphicsScene::AllLayers);
    else
        m_scene->invalidate(QRectF(m_lasMousePos.x(),viewPortSceneRect.top(),1.0,viewPortSceneRect.height()).adjusted(-0.5,-0.5,0.5,0.5),QGraphicsScene::AllLayers);
}

// determine the  number of decimal to display in the ruler according to the current zoom
int  MYTyponRuler::decNumber(){
    int decimalNb = 0;
    double lvlOfDetail = QStyleOptionGraphicsItem::levelOfDetailFromTransform(QGraphicsView::transform() * 1000);

    if ( lvlOfDetail > 700 ){
        if ( qApp->workSpace()->unit == MYTyponUnits::INCHES )
            decimalNb = 1;
    }
    if ( lvlOfDetail > 2500 ){
        if ( qApp->workSpace()->unit == MYTyponUnits::INCHES )
            decimalNb = 2;
        if ( qApp->workSpace()->unit == MYTyponUnits::MM )
            decimalNb = 1;
    }

    return decimalNb;
}

// draw ruler ticks and values
void MYTyponRuler::drawBackground (QPainter* painter, const QRectF &rect){

    // fill ruler rect with color then set  limit the drawing zone to scene rect
    painter->fillRect(rect,QColor(240,240,240));
    QRectF rf = rect.intersected(sceneRect()).adjusted(-1.0,-1.0,1.0,1.0);
    painter->fillRect(rf,Qt::white);
    painter->setClipRect(sceneRect());

    // set pen
    QPen pen;
    pen.setColor(Qt::black);
    pen.setCosmetic(true);
    painter->setPen(pen);

    // variable used to store the bigger step to be display
    double ratio = 0;

    int decimalNb = decNumber();

    // update ratio depending of the currently used unit
    switch (qApp->workSpace()->unit ){
        case MYTyponUnits::MM:
            ratio = 100.0;
            break;
        case MYTyponUnits::INCHES:
            ratio = 10.0;
            break;
        case MYTyponUnits::MIL:
            ratio = 10000.0;
            break;
    }

    // get level of details from 125 to 4000
    double lvlOfDetail = QStyleOptionGraphicsItem::levelOfDetailFromTransform(QGraphicsView::transform() * 1000.0);

    QVarLengthArray<QLineF> lines;
    QVarLengthArray<QLineF> subLines;

    // change main ruler tick depending of the zoom scale
    if ( lvlOfDetail > 250.0 ){
        ratio /= 10.0;
    }

    // treat mm slightly differntly to get a good aspect for the ruler
    if ( lvlOfDetail > 700.0 ){
        if ( qApp->workSpace()->unit!= MYTyponUnits::MM )
            ratio /= 10.0;
    }

    if ( lvlOfDetail > 900.0 ){
        if ( qApp->workSpace()->unit == MYTyponUnits::MM )
            ratio /= 10.0;
    }

    if ( lvlOfDetail > 2500.0 ){
        ratio /= 10.0;
    }

    // get raw step for main tick depending of the unit
    double rawStep = MYTyponUnits::unitToScene(qApp->workSpace()->unit,ratio);
    // calculate ruler ticks if orienation is horizontal
    if ( m_orientation == Horizontal ){
        double left = 0;
        // get offset of orign
        double xOffset = fmod(qApp->workSpace()->originPos(qApp->currentTypon()->typonSize()).x(), rawStep);
        // get offset of viewport with step
        left = rf.left() - fmod(rf.left(), rawStep) + xOffset - rawStep;
        // add main ticks
        for (double x = left; x < rf.right(); x += rawStep){
            lines.append(QLineF(x,rf.top(),x,rf.bottom()-5.0));
            // for each main tick, draw ten subticks, subtick number 5 is taller than other ones
            double divStep = rawStep / 10.0;
            for (int i = 1; i < 10; i++){
                double x2 = x + i * divStep;
                if ( i == 5 )
                    subLines.append(QLineF(x2,rf.top(),x2,rf.bottom()-10.0));
                else
                    subLines.append(QLineF(x2,rf.top(),x2,rf.bottom()-15.0));
            }

            // For each main tick write value in current unit
            QPoint pt = mapFromScene(x,0);
            QString t = QString::number(MYTyponUnits::sceneToUnit(qApp->workSpace()->unit,
                                      x -qApp->workSpace()->originPos(qApp->currentTypon()->typonSize()).x()),'f',decimalNb);
            QStaticText str(t);
            painter->save();
            painter->resetMatrix();
#ifdef W_OS_LINUX
            painter->drawStaticText(QRect(pt.x()+5,21-str.size().height()+4,100,m_height).topLeft(),str);
#else
            painter->drawStaticText(QRect(pt.x()+5,8,100,m_height).topLeft(),str);
#endif
            painter->restore();
        }
    }

    // if orientation is vertical
    if ( m_orientation == Vertical ){
        double top = 0.0;
        //get origin offset;
        double yOffset = fmod(qApp->workSpace()->originPos(qApp->currentTypon()->typonSize()).y(),rawStep);
        // get viewport offset with the step
        top = rf.top() - fmod(rf.top(), rawStep) + yOffset - rawStep;
        for (double y = top; y < rf.bottom(); y += rawStep){
            lines.append(QLineF(rf.left(),y,rf.right()-5.0,y));
            // for each main tick, draw ten subticks, subtick number 5 is taller than other ones
            double divStep = rawStep / 10.0;
            for ( int i = 1; i < 10; i++){
                double y2 = y + i * divStep;
                if ( i == 5 )
                    subLines.append(QLineF(rf.left(),y2,rf.right()-10.0,y2));
                else
                    subLines.append(QLineF(rf.left(),y2,rf.right()-15.0,y2));
            }

            // For each main tick write value in current unit
            QPoint pt = mapFromScene(0,y);

            painter->save();
            painter->resetMatrix();

            double xStr = m_width / 2;
            double yStr = pt.y();

            painter->translate(xStr, yStr);
            painter->rotate(-90);
            painter->translate(-xStr, -yStr);
            QString t = QString::number(MYTyponUnits::sceneToUnit(qApp->workSpace()->unit,
                                                                  y -qApp->workSpace()->originPos(qApp->currentTypon()->typonSize()).y()),'f',decimalNb);
            QStaticText str(t);
#ifdef W_OS_LINUX
            painter->drawStaticText(QRect(-str.size().width(),pt.y()+15-str.size().height(),100,m_width).topLeft(),str);
#else
            painter->drawStaticText(QRect(-str.size().width(),pt.y()-3,100,m_width).topLeft(),str);
#endif
            painter->restore();
        }
    }

    // actually draw ticks
    pen.setColor(Qt::darkGray);
    painter->setPen(pen);
    painter->drawLines(lines.data(), lines.size());
    pen.setColor(Qt::lightGray);
    painter->setPen(pen);
    painter->drawLines(subLines.data(), subLines.size());
}

// draw lines representing the mouse position
void MYTyponRuler::drawForeground(QPainter* painter, const QRectF &rect){
    Q_UNUSED(rect)

    if ( ! m_linesVisible )
        return;

    QPen pen;
    pen.setColor(Qt::black);
    pen.setCosmetic(true);
    painter->setPen(pen);

    if ( m_orientation == Horizontal ) {
        painter->drawLine(QLineF(m_lasMousePos.x(),0,m_lasMousePos.x(),m_height));
    }else{
        painter->drawLine(QLineF(0,m_lasMousePos.y(),m_width,m_lasMousePos.y()));
    }
}

void MYTyponRuler::hideLines(){
    if ( ! m_linesVisible )
        return;
    m_linesVisible = false;
    m_scene->invalidate();
}

void MYTyponRuler::changeVisibility(){
    setVisible(qApp->workSpace()->showRulers);
}

void MYTyponRuler::refresh(){
     m_scene->invalidate();
}
