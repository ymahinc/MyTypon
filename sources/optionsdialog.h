#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

/*
 * Dialog to adjust general, grid and current typon options
 * */

#include <QDialog>
#include "global.h"
#include "myoptions.h"
#include "mainwindow.h"

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(MainWindow *parent = 0, QString page = "0");
    ~OptionsDialog();

private slots:
    void on_buttonBox_accepted();
    void changeOptionPage();

private:
    Ui::OptionsDialog *ui;

    MainWindow *m_parent;
};

#endif // OPTIONSDIALOG_H
