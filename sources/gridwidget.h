#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

/*
 * Widget displayed in options dialog that handles grid properties
 */

#include <QWidget>

namespace Ui {
class GridWidget;
}

class GridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GridWidget(QWidget *parent = 0);
    ~GridWidget();
    void saveGridSettings();

private slots:
    void onMainLinesCheckChanged(int checked);

private:
    Ui::GridWidget *ui;
};

#endif // GRIDWIDGET_H
