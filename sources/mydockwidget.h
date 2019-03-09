#ifndef MYDOCKWIDGET_H
#define MYDOCKWIDGET_H

/* custom dowkwidget that handle transparency when floating
 * Not implemented yet
 * */

#include <QDockWidget>

class MYDockWidget : public QDockWidget
{
public:
    MYDockWidget(const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0);
    MYDockWidget(QWidget * parent = 0, Qt::WindowFlags flags = 0);

private:
    void init();
};

#endif // MYDOCKWIDGET_H
