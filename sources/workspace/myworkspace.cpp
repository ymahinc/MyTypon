#include "myworkspace.h"
#include <QDebug>
#include <QSettings>
#include <QFileInfo>
#include "../utils/mytyponunits.h"

// when a new workspace is created, load it with factory settings
// and inform the main window
MYWorkSpace::MYWorkSpace(QObject *parent)
    : QObject(parent){
    grid = new MYTyponGrid(this);
    currentWSPFilePath = "defaultworkspace.wsp";
    restoreFactorySettings();
    emit workspaceFileChanged("defaultworkspace.wsp");
}

// when units are modified via options dialog or typon corner widget, update current workspace
void MYWorkSpace::onUnitChanged(MYTyponUnits::Units newUnit){
    unit = newUnit;
    emit workspaceUnitChanged();
}

// load factory settings from ressources
void MYWorkSpace::restoreFactorySettings(){
    QSettings factorySettings(":/factoryconfig.ini", QSettings::IniFormat);
    m_widgetsState = factorySettings.value("windowState").toByteArray();

    linesUnderCursor = factorySettings.value("ShowLines").toBool();
    showRulers = factorySettings.value("ShowRulers").toBool();
    m_originPoint.setX(factorySettings.value("xOriginPoint").toDouble());
    m_originPoint.setY(factorySettings.value("yOriginPoint").toDouble());
    setOriginType((MYWorkSpace::OriginPositionTypes)factorySettings.value("OriginType").toInt(),m_originPoint);
    defaultTyponSize.setWidth(factorySettings.value("TyponWidth").toDouble());
    defaultTyponSize.setHeight(factorySettings.value("TyponHeight").toDouble());
    copperThickness = factorySettings.value("copperThickness").toDouble();
    tempRise = factorySettings.value("tempRise").toInt();
    zoomOnWheel = factorySettings.value("ZoomOnWheel").toBool();   
    semiTransparentTracks = factorySettings.value("transparentTracks").toBool();

    unit = (MYTyponUnits::Units)factorySettings.value("Units").toInt();

    factorySettings.beginGroup("Libraries");
    librariesPaths = factorySettings.value("LibrariesPaths").toStringList();
    factorySettings.endGroup();

    factorySettings.beginGroup("layers");
    drillColor = QColor(factorySettings.value("drill").toString());
    TSColor = QColor(factorySettings.value("topserigraphy").toString());
    TCColor = QColor(factorySettings.value("topcopper").toString());
    BCColor = QColor(factorySettings.value("bottomcopper").toString());
    BSColor = QColor(factorySettings.value("bottomserigraphy").toString());
    viaColor = QColor(factorySettings.value("via").toString());
    factorySettings.endGroup();

    factorySettings.beginGroup("grid");
    grid->setEnabled(factorySettings.value("Show").toBool());
    snapToGrid = factorySettings.value("Snap").toBool();
    grid->setGridType((MYTyponGrid::GridTypes)factorySettings.value("Style").toInt());
    grid->setXStepGrid(factorySettings.value("xStep").toDouble());
    grid->setYStepGrid(factorySettings.value("yStep").toDouble());
    grid->setGridColor(QColor(factorySettings.value("color").toString()));
    grid->setBackgroundColor(QColor(factorySettings.value("backgroundColor").toString()));
    grid->setMainLines(factorySettings.value("mainLines").toBool());
    grid->setMainLinesSubDiv(factorySettings.value("mainLinesSubDiv").toInt());
    grid->setDynamicGrid(factorySettings.value("dynamicGrid").toBool());
    factorySettings.endGroup();
    emit workspaceUpdated();
}

// handles the origin type (center of board, topleft of typon, etc...)
// so that the rulers and position of items can be determined correctly
MYWorkSpace::OriginPositionTypes MYWorkSpace::originType(){
    return m_originType;
}

void MYWorkSpace::setOriginType(OriginPositionTypes type, QPointF customPoint){
    m_originType = type;
    m_originPoint = customPoint;
}

// calculate origin pos in scene coordinate, according to the size of the typon passed as an argument
// this size can be default size store in workspace or size of typon in the currently used document
QPointF MYWorkSpace::originPos(QSizeF typonSize){
    QPointF originPos;
    switch (m_originType) {
        case(WSC):
            originPos = QPointF(25000,25000);
            break;
        case(WSTL):
            originPos = QPointF(0,0);
            break;
        case(WSBL):
            originPos = QPointF(0,50000);
            break;
        case(WSTR):
            originPos = QPointF(50000,0);
            break;
        case(WSBR):
            originPos = QPointF(50000,50000);
            break;
        case(TC):
            originPos = QPointF(25000,25000);
            break;
        case(TTL):
            originPos = QPointF(25000,25000) - QPointF(typonSize.width()/2.0,typonSize.height()/2.0);
            break;
        case(TBL):
            originPos = QPointF(25000,25000) - QPointF(typonSize.width()/2.0,-typonSize.height()/2.0);
            break;
        case(TTR):
            originPos = QPointF(25000,25000) - QPointF(-typonSize.width()/2.0,typonSize.height()/2.0);
            break;
        case(TBR):
            originPos = QPointF(25000,25000) - QPointF(-typonSize.width()/2.0,-typonSize.height()/2.0);
            break;
        case(CUSTOM):
            originPos = QPointF(m_originPoint.x(),m_originPoint.y());
            break;
    }
    m_originPoint = originPos;
    return originPos;
}

// load workspace from previously saved file
void MYWorkSpace::loadFromFile(QString filePath){
    currentWSPFilePath = filePath;

    QSettings wspSettings(filePath,QSettings::IniFormat);

    showRulers = wspSettings.value("ShowRulers").toBool();
    linesUnderCursor = wspSettings.value("ShowLines").toBool();
    QPointF originPoint(wspSettings.value("xOriginPoint").toDouble(),wspSettings.value("yOriginPoint").toDouble());
    setOriginType((MYWorkSpace::OriginPositionTypes)wspSettings.value("OriginType").toInt(),originPoint);
    QSizeF typonSize;
    typonSize.setWidth(wspSettings.value("TyponWidth").toDouble());
    typonSize.setHeight(wspSettings.value("TyponHeight").toDouble());
    defaultTyponSize = typonSize;
    copperThickness = wspSettings.value("copperThickness").toDouble();
    zoomOnWheel = wspSettings.value("ZoomOnWheel").toBool();
    semiTransparentTracks = wspSettings.value("transparentTracks").toBool();
    unit = (MYTyponUnits::Units)wspSettings.value("Units").toDouble();
    tempRise = wspSettings.value("tempRise").toInt();

    m_widgetsState  = wspSettings.value("windowState").toByteArray();

    wspSettings.beginGroup("Libraries");
    librariesPaths = wspSettings.value("LibrariesPaths").toStringList();
    wspSettings.endGroup();

    wspSettings.beginGroup("layers");
    drillColor = QColor(wspSettings.value("drill").toString());
    TSColor = QColor(wspSettings.value("topserigraphy").toString());
    TCColor = QColor(wspSettings.value("topcopper").toString());
    BCColor = QColor(wspSettings.value("bottomcopper").toString());
    BSColor = QColor(wspSettings.value("bottomserigraphy").toString());
    viaColor = QColor(wspSettings.value("via").toString());
    wspSettings.endGroup();

    wspSettings.beginGroup("grid");
    grid->setEnabled(wspSettings.value("Show").toBool());
    snapToGrid = wspSettings.value("Snap").toBool();
    grid->setGridType((MYTyponGrid::GridTypes)wspSettings.value("Style").toInt());
    grid->setXStepGrid(wspSettings.value("xStep").toDouble());
    grid->setYStepGrid(wspSettings.value("yStep").toDouble());
    grid->setGridColor(QColor(wspSettings.value("color").toString()));
    grid->setBackgroundColor(QColor(wspSettings.value("backgroundColor").toString()));
    grid->setMainLines(wspSettings.value("mainLines").toBool());
    grid->setMainLinesSubDiv(wspSettings.value("mainLinesSubDiv").toInt());
    grid->setDynamicGrid(wspSettings.value("dynamicGrid").toBool());
    wspSettings.endGroup();

    emit workspaceUpdated();

    QFileInfo fileInfo(filePath);
    emit workspaceFileChanged(fileInfo.fileName());
}

// write this workspace to the file given as parameter
void MYWorkSpace::writeToFile(QString filePath, QByteArray widgetsState){
    QSettings settings(filePath, QSettings::IniFormat);

    // if widgetsState is empty we save current state: probably factory one
    if ( widgetsState.isEmpty() )
        settings.setValue("windowState", m_widgetsState);
    else
        settings.setValue("windowState", widgetsState);

    settings.setValue("ShowLines",linesUnderCursor);
    settings.setValue("ShowRulers",showRulers);
    settings.setValue("OriginType",(int)originType());
    settings.setValue("xOriginPoint",m_originPoint.x());
    settings.setValue("yOriginPoint",m_originPoint.y());
    settings.setValue("TyponWidth",defaultTyponSize.width());
    settings.setValue("TyponHeight",defaultTyponSize.height());
    settings.setValue("copperThickness",copperThickness);
    settings.setValue("tempRise",tempRise);
    settings.setValue("Units",(int)unit);
    settings.setValue("ZoomOnWheel",zoomOnWheel);
    settings.setValue("transparentTracks",semiTransparentTracks);

    settings.beginGroup("layers");
    settings.setValue("drill",drillColor.name());
    settings.setValue("topserigraphy",TSColor.name());
    settings.setValue("topcopper",TCColor.name());
    settings.setValue("bottomcopper",BCColor.name());
    settings.setValue("bottomserigraphy",BSColor.name());
    settings.setValue("via",viaColor.name());
    settings.endGroup();

    settings.beginGroup("Libraries");
    settings.setValue("LibrariesPaths",librariesPaths);
    settings.endGroup();

    settings.beginGroup("grid");
    settings.setValue("Show",grid->enabled());
    settings.setValue("Snap",snapToGrid);
    settings.setValue("Style",(int)grid->gridType());
    settings.setValue("xStep",grid->xStepGrid());
    settings.setValue("yStep",grid->yStepGrid());
    settings.setValue("color",grid->gridColor().name());
    settings.setValue("backgroundColor",grid->backgroundColor().name());
    settings.setValue("mainLines",grid->mainLines());
    settings.setValue("mainLinesSubDiv",grid->mainLinesSubDivs());
    settings.setValue("dynamicGrid",grid->dynamicGrid());
    settings.endGroup();

    QFileInfo fileInfo(filePath);
    emit workspaceFileChanged(fileInfo.fileName());
}

QByteArray MYWorkSpace::widgetsState(){
    return m_widgetsState;
}
