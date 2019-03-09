#ifndef MYTYPONGRID_H
#define MYTYPONGRID_H

/* class represented the typon grid */

#include <QColor>
#include <QPoint>
#include <QPointF>
#include <QObject>

class MYTyponGrid : public QObject
{
    Q_OBJECT

public:
    enum GridTypes{ Dot, Line };

    MYTyponGrid(QObject *parent = 0);

    void setEnabled(bool enabled);
    bool enabled();
    void setBackgroundColor(QColor color);
    QColor backgroundColor();
    void setGridColor(QColor color);
    QColor gridColor();
    void setGridType(GridTypes type);
    GridTypes gridType();
    void setXStepGrid(double step);
    double xStepGrid();
    void setYStepGrid(double step);
    double yStepGrid();
    QPoint gridStep();
    void setStepGrid(double xStep, double yStep);
    void setStepGrid(QPointF value);
    QPointF nearestPoint(QPointF point, QPointF originPoint, bool ignoreSnap, double factor = 1.0, QPointF offset = QPointF(0.0,0.0));
    bool mainLines();
    void setMainLines(bool on);
    int mainLinesSubDivs();
    bool dynamicGrid();
    void setDynamicGrid(bool on);

public slots:
    void setMainLinesSubDiv(int subDivs);

private:
    bool m_enabled;
    QColor m_bgColor;
    QColor m_gridColor;
    GridTypes m_gridType;
    double m_xStepGrid;
    double m_yStepGrid;
    bool m_mainLines;
    bool m_dynamicGrid;
    int m_mainLinesSubDivs;
};

#endif // MYTYPONGRID_H
