#include "groundplaneeffect.h"
#include <QPainter>
#include <QBitmap>

GroundPlaneEffect::GroundPlaneEffect()
    : QGraphicsEffect() {
    m_brush = QBrush(Qt::black);
}

void GroundPlaneEffect::setOpacityMask(QBrush mask){
    m_brush = mask;
}

QRectF GroundPlaneEffect::boundingRectFor(const QRectF& sourceRect) const{
    return sourceRect;
}

void GroundPlaneEffect::draw(QPainter* painter){
/*
    Q_D(QGraphicsOpacityEffect);

    // Transparent; nothing to draw.
    if (d->isFullyTransparent)
        return;

    // Opaque; draw directly without going through a pixmap.
    if (d->isFullyOpaque && !d->hasOpacityMask) {
        drawSource(painter);
        return;
    }
*/
    QPoint offset;
    Qt::CoordinateSystem system = sourceIsPixmap() ? Qt::LogicalCoordinates : Qt::DeviceCoordinates;
    QPixmap pixmap = sourcePixmap(system, &offset, QGraphicsEffect::NoPad);
    if (pixmap.isNull())
        return;

    painter->save();
    painter->setOpacity(1.0);

    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHints(painter->renderHints());
    pixmapPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    if (system == Qt::DeviceCoordinates) {
       QTransform worldTransform = painter->worldTransform();
       worldTransform *= QTransform::fromTranslate(-offset.x(), -offset.y());
       pixmapPainter.setWorldTransform(worldTransform);
       pixmapPainter.fillRect(sourceBoundingRect(), m_brush);
    } else {
       pixmapPainter.translate(-offset);
       pixmapPainter.fillRect(pixmap.rect(), m_brush);
    }

    QPixmap finalPix = pixmap;
    QPixmap mask = pixmap.createMaskFromColor(m_brush.color());
    finalPix.setMask(mask);

    if (system == Qt::DeviceCoordinates)
       painter->setWorldTransform(QTransform());

    painter->drawPixmap(offset, finalPix);
    painter->restore();
}
