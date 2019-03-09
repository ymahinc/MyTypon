#ifndef GENERALOPTIONSWIDGET_H
#define GENERALOPTIONSWIDGET_H

/*
 * Widget displayed in option dialog that handle general workspace options
 */

#include <QWidget>
#include "global.h"

namespace Ui {
class GeneralOptionsWidget;
}

class GeneralOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralOptionsWidget(QWidget *parent = 0);
    ~GeneralOptionsWidget();
    void saveWorkSpace();
    void saveOptions();

private slots:
    void updateOriginPointWidgets(int index);

private:
    Ui::GeneralOptionsWidget *ui;
};

#endif // GENERALOPTIONSWIDGET_H
