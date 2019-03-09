#ifndef MYUNITSMENULABEL_H
#define MYUNITSMENULABEL_H

/*
 * this is the corner widget of the typon: a label that display a menu to update unit used by application
 * */

#include <QLabel>
#include <QMenu>
#include <QActionGroup>
#include <QAction>
#include <QMouseEvent>
#include "mytyponunits.h"

class MYUnitsMenuLabel : public QLabel
{
    Q_OBJECT

public:
    MYUnitsMenuLabel(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

public slots:
    void changeVisibility();

signals:
    void unitChanged(MYTyponUnits::Units unit);

protected :
    virtual void mousePressEvent(QMouseEvent *ev);

private slots:
    void onMenuChanged(QAction *action);
    void onUnitChanged();

private:
    QMenu *m_menu;
    QAction  *m_mmAction;
    QAction *m_inAction;
    QAction *m_milAction;
    QActionGroup *m_unitsActionGroup;

    void setTextFromUnit(MYTyponUnits::Units unit);
};

#endif // MYUNITSMENULABEL_H
