#ifndef MYTOOLBOX_H
#define MYTOOLBOX_H

/*
 * Widget that display tools and tools options
 * got a header that is a flow layout where tools can be selected
 * and a stacked widget which each tools option on each page
 */

#include <QWidget>
#include "flowlayout.h"
#include <QButtonGroup>
#include "tools/mytoolbase.h"

namespace Ui {
class MYToolBox;
}

class MYToolBox : public QWidget
{
    Q_OBJECT

public:
    explicit MYToolBox(QWidget *parent = 0);
    ~MYToolBox();
    void addTool(MYToolBase *tool);   
    QWidget *widget(int index);

public slots:
    void setCurrentIndex(int index);

signals:
    void currentChanged(int);

private:
    Ui::MYToolBox *ui;

    FlowLayout *m_flowLayout;
    QButtonGroup *m_buttonGroup;
};

#endif // MYTOOLBOX_H
