#include "workarea.h"

#include <QPainter>

WorkArea::WorkArea(QGraphicsItem *parent)
    : QGraphicsObject(parent)
{
    this->setRect(QRectF(0, 0, 500, 500));
}

QRectF WorkArea::rect() const
{
    return _boundingRect;
}

void WorkArea::setRect(const QRectF &rect)
{
    if(rect != _boundingRect) {
        this->prepareGeometryChange();
        _boundingRect = rect;
    }
}

QRectF WorkArea::boundingRect() const
{
    qreal penWidth = 1; // esto debería ser variable
    return QRectF(_boundingRect.x() - penWidth / 2, _boundingRect.y() - penWidth / 2,
                  _boundingRect.width() + penWidth, _boundingRect.height() + penWidth);
}

void WorkArea::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen p;
    p.setWidth(0);
    painter->setPen(p);

    painter->drawRect(_boundingRect);

    qreal x = _boundingRect.x();
    qreal y = _boundingRect.y();
    qreal height = _boundingRect.height();
    qreal width = _boundingRect.width();

    for(int i = x; i < width + x; i += 25) {
        painter->drawLine(i, y, i, height + y);
    }

    for(int i = y; i < height + y; i += 25) {
        painter->drawLine(x, i, width + x, i);
    }
}
