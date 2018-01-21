#include "visualgroup.h"

#include <QPainter>

VisualGroup::VisualGroup(QGraphicsItem *parent)
    : QGraphicsObject(parent)
{}

VisualGroup::VisualGroup(QGraphicsItem *parent, const QRectF &rect)
    : QGraphicsObject(parent)
{
    this->setRect(rect);
}

QRectF VisualGroup::rect() const
{
    return _boundingRect;
}

void VisualGroup::setRect(const QRectF &rect)
{
    if(rect != _boundingRect) {
        this->prepareGeometryChange();
        _boundingRect = rect;
    }
}

QRectF VisualGroup::boundingRect() const
{
    qreal penWidth = 1; // esto debería ser variable
    return QRectF(_boundingRect.x() - penWidth / 2, _boundingRect.y() - penWidth / 2,
                  _boundingRect.width() + penWidth, _boundingRect.height() + penWidth);
}

void VisualGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(Qt::lightGray);
    QPen p;
    p.setWidth(0);
    painter->setPen(p);

    painter->drawRect(_boundingRect);
}
