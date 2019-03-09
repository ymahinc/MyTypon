#ifndef LAYERSOPTIONWIDGET_H
#define LAYERSOPTIONWIDGET_H

#include <QWidget>

namespace Ui {
class LayersOptionWidget;
}

class LayersOptionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LayersOptionWidget(QWidget *parent = 0);
    ~LayersOptionWidget();
    QColor drillColor;
    QColor TSColor;
    QColor TCColor;
    QColor BCColor;
    QColor BSColor;
    QColor viaColor;

private slots:
    void onDoubleClick(int row, int column);

private:
    Ui::LayersOptionWidget *ui;
};

#endif // LAYERSOPTIONWIDGET_H
