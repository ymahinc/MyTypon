#ifndef TEXTITEM_H
#define TEXTITEM_H

/*
 * item that handle text
 */

#include "../myitembase.h"
#include <QFont>
class TextTool;

class TextItem : public MYItemBase
{
public:
    TextItem(MYItemBase *parent = 0, bool incrementCounter = false);

    double width() const;
    TextItem *clone();
    void setWeight(double weight);
    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    QPainterPath shape() const;
    void setText(QString text);
    QString text() const;
    void setFont(QFont font);
    QFont font() const;
    void setTool(TextTool *tool);
    TextTool *tool();
    void setBold(bool bold);
    bool bold() const;
    void setItalic(bool italic);
    bool italic() const;
    void setUnderlined(bool underlined);
    bool underlined() const;
    void save(QDataStream &stream);
    int restore(QDataStream &stream);
    double weight();

private:
    void updateGeometry();
    QRectF m_bRect;
    QPainterPath m_shape;
    double m_height;
    QString m_text;
    QFont m_font;
    QPainterPath m_path;
    TextTool *m_tool;
    bool m_bold;
    bool m_italic;
    bool m_underlined;
};

#endif // TEXTITEM_H
