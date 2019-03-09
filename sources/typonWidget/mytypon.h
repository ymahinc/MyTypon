#ifndef MYTYPON_H
#define MYTYPON_H

// the widget wich represent a typon:
// - a vertical ruler
// - an horizontal ruler
// - a corner menu label widget to change unities
// - a typonview wich is the drawing widget

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QActionGroup>
#include "mytyponscene.h"
#include "mytyponview.h"
#include "ruler/mytyponruler.h"
#include <QVector>
#include <QUndoStack>
#include <QShowEvent>
#include "../utils/myunitsmenulabel.h"

namespace Ui {
class MYTypon;
}

class MYTypon : public QWidget
{
    Q_OBJECT

public:
    explicit MYTypon(QWidget *parent = 0, QString path = QString());
    ~MYTypon();

    double copperThickness();
    void setCopperThickness(double thickness);
    void setTyponSize(QSizeF size);
    QSizeF typonSize();
    MYTyponScene *typonScene();
    MYTyponView *typonView();
    QString typonName();
    void setTyponName(QString name);
    QUndoStack *undoStack();
    void scale(double scaleValue);
    quint64 trackContainerCounter = 0;
    quint64 textCounter = 0;
    quint64 padCounter = 0;
    quint64 groupCounter = 0;
    quint64 trackCounter = 0;
    quint64 rectCounter = 0;
    quint64 ellipseCounter = 0;
    quint64 polygonCounter = 0;
    void save(QDataStream &stream);
    void restore(QDataStream &stream, QString path);
    QString filePath();
    void setFilePath(QString path);
    void restoreItem(QDataStream &stream, int type, MYItemBase *grpItem);

signals:
    void mousePosHasChanged(QPointF pos);
    void viewportLeaved();
    void unitsUpdated();
    void typonNameHasChanged(QString name);
    void zoomWidgetsUpdateRequired();

public slots:
    void zoomIn();
    void zoomOut();
    void zoomReset();
    void zoomBoard();
    void zoomToRect(QRectF rect, bool zoomIn = true);
    void zoomFit();
    void zoomSelection();
    void updateTyponName(bool clean);
    void copy();
    void paste();

protected:
     void showEvent(QShowEvent *event);

private slots:
    void center();
    void updateLayersTransparency();
    void updateScene();

private:
    void hideRulers(bool hide);

    Ui::MYTypon *ui;

    QGridLayout *m_layout;
    MYTyponView* m_view;
    MYTyponScene* m_scene;
    MYTyponRuler *m_hRuler;
    MYTyponRuler *m_vRuler;
    MYUnitsMenuLabel *m_unitsLabel;
    QSizeF m_typonSize;
    double m_copperThickness;
    QUndoStack *m_undoStack;
    QUndoStack *m_loadUndoStack;
    bool m_firstShow;
    static int instancesCounter;
    QString m_typonName;
    QString m_path;
};

#endif // MYTYPON_H
