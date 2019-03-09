#include "mytypongrid.h"
#include <QDebug>
#include "../utils/mytyponunits.h"

MYTyponGrid::MYTyponGrid(QObject *parent)
    :   QObject(parent) {
    m_enabled = true;
    m_bgColor = Qt::black;
    m_gridColor = QColor(61,61,61);
    double gridStep = 50;
    setStepGrid(gridStep,gridStep);
    m_gridType = Dot;
    m_mainLines = false;
    m_dynamicGrid = false;
    m_mainLinesSubDivs = 5;
}

void MYTyponGrid::setEnabled(bool enabled){
    m_enabled = enabled;
}

bool MYTyponGrid::enabled(){
    return m_enabled;
}

void MYTyponGrid::setBackgroundColor(QColor color){
    m_bgColor = color;
}

QColor MYTyponGrid::backgroundColor(){
    return m_bgColor;
}

void MYTyponGrid::setGridColor(QColor color){
    m_gridColor = color;
}

QColor MYTyponGrid::gridColor(){
    return m_gridColor;
}

// grid type: line or dots
void MYTyponGrid::setGridType(GridTypes type){
    m_gridType = type;
}

MYTyponGrid::GridTypes MYTyponGrid::gridType(){
    return m_gridType;
}

void MYTyponGrid::setXStepGrid(double step){
    m_xStepGrid = step;
}

double MYTyponGrid::xStepGrid(){
    return m_xStepGrid;
}

void MYTyponGrid::setYStepGrid(double step){
    m_yStepGrid = step;
}

double MYTyponGrid::yStepGrid(){
    return m_yStepGrid;
}

void MYTyponGrid::setStepGrid(double xStep, double yStep){
    setXStepGrid(xStep);
    setYStepGrid(yStep);
}

void MYTyponGrid::setStepGrid(QPointF value){
    setStepGrid(value.x(),value.y());
}

QPoint MYTyponGrid::gridStep(){
    return QPoint(m_xStepGrid,m_yStepGrid);
}

// return nearest grid point, according to originpoint and an optional offset from this nearest grid point
// if ignoresnap is true, return point with no modification
QPointF MYTyponGrid::nearestPoint(QPointF point, QPointF originPoint, bool ignoreSnap, double factor, QPointF offset){
    QPointF newPoint = point;
    if (newPoint.x() < 0 )
        newPoint.setX(0);
    if (newPoint.x() > 50000 )
        newPoint.setX(50000);
    if (newPoint.y() < 0 )
        newPoint.setY(0);
    if (newPoint.y() > 50000 )
        newPoint.setY(50000);
    if ( ! ignoreSnap ){ // we want to snap on the grid
        double xStep = m_xStepGrid * factor;
        double yStep = m_yStepGrid * factor;
        // get origin point offsets
        double xOriginOffset = fmod(originPoint.x(), xStep);
        double yOriginOffset = fmod(originPoint.y(), yStep);
        // get point offsets
        double xOffset = fmod(newPoint.x(), xStep);
        double yOffset = fmod(newPoint.y(), yStep);
        // get nearest grid point
        double xResultOffset = - xOffset + xOriginOffset;
        double yResultOffset = - yOffset + yOriginOffset;
        // round values to left/right or bottom/tom point if we are more than a half grid step
        if ( xResultOffset > xStep / 2.0 )
            xResultOffset -= xStep;
        if ( xResultOffset < xStep / -2.0 )
            xResultOffset += xStep;
        if ( yResultOffset > yStep / 2.0 )
            yResultOffset -= yStep;
        if ( yResultOffset < yStep / -2.0 )
            yResultOffset += yStep;
        double left = newPoint.x() + xResultOffset;
        double top = newPoint.y() + yResultOffset;
        // return calculated point with optionnal offset
        newPoint.setX(left+offset.x());
        newPoint.setY(top+offset.y());
    }
    return newPoint;
}

// draw or not main lines every x step
bool MYTyponGrid::mainLines(){
    return m_mainLines;
}

void MYTyponGrid::setMainLines(bool on){
    m_mainLines = on;
}

// number of steps beetween each main line
int MYTyponGrid::mainLinesSubDivs(){
    return m_mainLinesSubDivs;
}

void MYTyponGrid::setMainLinesSubDiv(int subDivs){
    m_mainLinesSubDivs = subDivs;
}

// determine if the grid is fixed or always displayed with a dynamic step
bool MYTyponGrid::dynamicGrid(){
    return m_dynamicGrid;
}

void MYTyponGrid::setDynamicGrid(bool on){
    m_dynamicGrid = on;
}
