#include "visualgroup.h"

#include <QPainter>

VisualGroup::VisualGroup(QGraphicsItem *parent)
    : QGraphicsObject(parent)
{}

VisualGroup::VisualGroup(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size)
    : QGraphicsObject(parent), _boundingRect(QRectF(QPointF(0, 0), size))
{
    this->setPos(pos);
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
    return _boundingRect;
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
