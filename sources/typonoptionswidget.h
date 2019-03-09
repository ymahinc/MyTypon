#ifndef TYPONOPTIONSWIDGET_H
#define TYPONOPTIONSWIDGET_H

/* widget displayed in options dialog
 * handles current typon board size and copper thickness
 */

#include <QWidget>

namespace Ui {
class TyponOptionsWidget;
}

class TyponOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TyponOptionsWidget(QWidget *parent = 0);
    ~TyponOptionsWidget();

    void setTyponSize(QSizeF size);
    QSizeF typonSize();
    void setCopperThickness(double value);
    double copperThickness();

private:
    Ui::TyponOptionsWidget *ui;
};

#endif // TYPONOPTIONSWIDGET_H
