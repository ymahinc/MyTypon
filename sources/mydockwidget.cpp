#include "mydockwidget.h"

MYDockWidget::MYDockWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(title,parent,flags){
    init();
}

MYDockWidget::MYDockWidget(QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(parent,flags){
    init();
}

void MYDockWidget::init(){
    /*QFont f = font();
    f.setPointSize(24);
    setFont(f);*/
}
