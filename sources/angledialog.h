#ifndef ANGLEDIALOG_H
#define ANGLEDIALOG_H

#include <QDialog>

namespace Ui {
class AngleDialog;
}

class AngleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AngleDialog(QWidget *parent = 0);
    ~AngleDialog();
    double getAngle();

private:
    Ui::AngleDialog *ui;
};

#endif // ANGLEDIALOG_H
