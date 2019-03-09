#ifndef LIBRARIESWIDGET_H
#define LIBRARIESWIDGET_H

#include <QWidget>

namespace Ui {
class LibrariesWidget;
}

class LibrariesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LibrariesWidget(QWidget *parent = 0);
    ~LibrariesWidget();
    void saveWorkSpace();

private slots:
    void addLibrary();
    void removeLibrary();
    void moveUp();
    void moveDown();
    void updateButtons();

private:
    Ui::LibrariesWidget *ui;

    QStringList librariesPaths();
};

#endif // LIBRARIESWIDGET_H
