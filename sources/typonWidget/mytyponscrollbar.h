#ifndef MYTYPONSCROLLBAR_H
#define MYTYPONSCROLLBAR_H

// custom scrollbars used to change displayed cursor
// and emit a viewport leave signal on the typon view widget

#include <QScrollBar>
#include <QMouseEvent>
#include "mytyponview.h"

class MYTyponScrollBar : public QScrollBar
{
public:
    MYTyponScrollBar(QWidget * parent = 0);

protected:
    void mouseMoveEvent(QMouseEvent * e);

private:
    MYTyponView *m_view;
};

#endif // MYTYPONSCROLLBAR_H
