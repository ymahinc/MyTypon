#ifndef TEXTTOOL_H
#define TEXTTOOL_H

/*
 * Tool taht provides a texItem and the corresponding editing widget
 */

#include <QWidget>
#include "../mytoolbase.h"
#include "items/textitem/textitem.h"

namespace Ui {
class TextTool;
}

class TextTool : public MYToolBase
{
    Q_OBJECT

public:
    explicit TextTool(QWidget *parent = 0, TextItem *item = 0);
    ~TextTool();

    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleWheelEvent(QWheelEvent *event);
    void handleKeyPressEvent(QKeyEvent *event);
    void handleKeyReleaseEvent(QKeyEvent *event);
    void handleEnterEvent(QEvent *event);
    void handleLeaveEvent(QEvent *event);

    void updateTool();

private slots:
    void on_boldCheckBox_toggled(bool checked);
    void on_italicCheckBox_toggled(bool checked);
    void on_underlineCheckBox_toggled(bool checked);
    void onCurrentFontChanged(QFont font);
    void onTextChanged();
    void onNameChanged();
    void updateWeight();

private:
    Ui::TextTool *ui;
    TextItem *m_textItem;
    QFont m_font;
};

#endif // TEXTTOOL_H
