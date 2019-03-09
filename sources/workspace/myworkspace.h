#ifndef MYWORKSPACE_H
#define MYWORKSPACE_H

// workspace stores all global preferences

#include "mytypongrid.h"
#include "utils/mytyponunits.h"
#include <QObject>
#include <QSizeF>
#include <QFile>
#include <QByteArray>

class MYWorkSpace : public QObject
{
    Q_OBJECT

public:
    enum OriginPositionTypes{
        WSC, // workspace center
        WSTL, // workspace top left
        WSBL, // workspace bottom left
        WSTR, // workspace top right
        WSBR, // workspace bottom right
        TC, // typon center
        TTL, // typon top left
        TBL, // typon bottom left
        TTR, // typon top right
        TBR,  // typon bottom right
        CUSTOM
    };

    MYWorkSpace(QObject *parent = 0);

    MYTyponGrid *grid;
    bool linesUnderCursor; // show lines under mouse cursor
    bool showRulers; // show rulers widgets
    // manage origin types
    OriginPositionTypes originType();
    QPointF originPos(QSizeF typonSize);
    void setOriginType(OriginPositionTypes type, QPointF customPoint = QPointF());

    QSizeF defaultTyponSize; // default board size for newly created typons
    bool snapToGrid; // snap or not to the grid when moving mouse
    double copperThickness; // copper thickness for newly created typons in order to calculated max current of a track
    void loadFromFile(QString filePath);
    void writeToFile(QString filePath, QByteArray widgetsState = QByteArray());
    QByteArray widgetsState();
    QString currentWSPFilePath;
    MYTyponUnits::Units unit; // units used to work
    bool zoomOnWheel; // mouse wheel provides zoom if true, if false scroll the view
    int tempRise; // temp rise to calculate max track current
    bool semiTransparentTracks;// are items semi transparents?
    QStringList librariesPaths;
    QColor drillColor;
    QColor TSColor;
    QColor TCColor;
    QColor BCColor;
    QColor BSColor;
    QColor viaColor;

public slots:
    void restoreFactorySettings();
    void onUnitChanged(MYTyponUnits::Units newUnit);

signals:
    void workspaceUpdated();
    void workspaceFileChanged(QString fileName);
    void workspaceUnitChanged();

private:
    OriginPositionTypes m_originType;
    QByteArray m_widgetsState;
    QPointF m_originPoint;
};

#endif // MYWORKSPACE_H
