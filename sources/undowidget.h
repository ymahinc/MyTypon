#ifndef UNDOWIDGET_H
#define UNDOWIDGET_H

// custom undo widget, just to adjust display as wanted

#include <QWidget>
#include <QUndoView>

namespace Ui {
class UndoWidget;
}

class UndoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UndoWidget(QWidget *parent = 0);
    ~UndoWidget();
    QUndoView *undoView();

private:
    Ui::UndoWidget *ui;
};

#endif // UNDOWIDGET_H
