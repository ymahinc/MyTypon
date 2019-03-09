#include "textitem.h"
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QFontMetricsF>
#include <QApplication>
#include "global.h"
#include "tools/TextTool/texttool.h"

TextItem::TextItem(MYItemBase *parent, bool incrementCounter)
    : MYItemBase(parent){
    // pointer to the edit widget for this item
    // could be 0 or could be a valid widget wich we need to update values when this item changes
    m_tool = 0;
    m_bold = false;
    m_italic = false;
    m_underlined = false;
    setFlag(QGraphicsItem::ItemIsSelectable,false);
    setFlag(QGraphicsItem::ItemIsMovable,false);
    m_bRect = QRectF();
    m_shape = QPainterPath();
    // if the item is a "real" one, not tool item or temporary item for undo/redo commands, increment counter
    if ( qApp->currentTypon() && incrementCounter )
        setItemName(QObject::tr("Text %1").arg(QString::number(++qApp->currentTypon()->textCounter)));
    m_font = QApplication::font();
    m_height = 150;
    m_bRect = QRectF(-50,-50,100,100);
}

void TextItem::save(QDataStream &stream){
    stream << 2 << m_name << pos() << transform() << rotation() << layer()->realLayerZValue() << m_font << m_text;
}

int TextItem::restore(QDataStream &stream){
    QString name;
    QPointF pos;
    QTransform transform;
    double rotation;
    int layerZValue;
    QFont font;
    QString text;

    stream >> name >> pos >> transform >> rotation >> layerZValue >> font >> text;
    setItemName(name);
    setPos(pos);
    setRotation(rotation);
    setTransform(transform,true);

    m_italic = font.italic();
    m_bold = font.bold();
    m_height = font.pointSizeF();
    m_underlined = font.underline();
    setFont(font);
    setText(text);

    return layerZValue;
}

void TextItem::setItalic(bool italic){
    m_italic = italic;
    m_font.setItalic(m_italic);
    updateGeometry();
}

bool TextItem::italic() const{
    return m_italic;
}

void TextItem::setUnderlined(bool underlined){
    m_underlined = underlined;
    m_font.setUnderline(m_underlined);
    updateGeometry();
}

bool TextItem::underlined() const{
    return m_underlined;
}

void TextItem::setBold(bool bold){
    m_bold = bold;
    m_font.setBold(bold);
    updateGeometry();
}

bool TextItem::bold() const{
    return m_bold;
}

void TextItem::setTool(TextTool *tool){
    m_tool = tool;
}

TextTool *TextItem::tool(){
    return m_tool;
}

void TextItem::setText(QString text){
    m_text = text;
    updateGeometry();
}

QString TextItem::text() const{
    return m_text;
}

void TextItem::setFont(QFont font){
    m_font = font;
    m_font.setItalic(m_italic);
    m_font.setBold(m_bold);
    m_font.setUnderline(underlined());
    m_font.setPointSizeF(m_height);
    updateGeometry();
}

QFont TextItem::font() const{
    return m_font;
}

QPainterPath TextItem::shape() const{
    return m_shape;
}

//return correct bounding rect for display update
QRectF TextItem::boundingRect() const {
    if ( ! isVisible() )
        return QRectF();
    else
        return m_bRect;
}

double TextItem::weight(){
    return m_height;
}

// clone the item, used when adding a new item to the current scene
TextItem *TextItem::clone(){
    TextItem *clone = new TextItem(0,true);
    clone->prepareGeometryChange();
    clone->setBold(m_bold);
    clone->setItalic(m_italic);
    clone->setUnderlined(m_underlined);
    clone->setText(m_text);
    clone->setWeight(m_height);
    clone->setFont(m_font);
    clone->setGroup(group());
    clone->setTransform(transform());
    clone->setRotation(rotation());
    return clone;
}

void TextItem::setWeight(double weight){
    m_height = weight;
    m_font.setPointSizeF(weight);
    updateGeometry();
}

double TextItem::width() const{
    return m_height;
}

// properties of this item have changed, let update shape
void TextItem::updateGeometry(){
    prepareGeometryChange();

    m_shape = QPainterPath();
    m_shape.addText(QPointF(0,0),m_font,m_text);
    m_bRect = m_shape.boundingRect();
    m_shape.translate(-m_bRect.width()/2.0,m_bRect.height()/2.0);
    m_bRect = m_shape.boundingRect();
}

void TextItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget){
    Q_UNUSED(widget);
    Q_UNUSED(option)
    if ( ! layer() )
        return;
    painter->setClipRect( option->exposedRect );

    painter->setPen(Qt::NoPen);

    QColor color;
    if ( isToolItem() )
        color = layer()->color().lighter();
    else
        color = layer()->color();

    if ( m_isSelected )
        color = Qt::white;

    painter->fillPath(m_shape,color);
}
