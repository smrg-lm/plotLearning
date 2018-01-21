#include "rulearea.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

RuleArea::RuleArea(QGraphicsItem *parent)
    : QGraphicsObject(parent)
{}

QRectF RuleArea::rect() const
{
    return _boundingRect;
}

void RuleArea::setRect(const QRectF &rect)
{
    if(rect != _boundingRect) {
        this->prepareGeometryChange();
        _boundingRect = rect;
    }
}

QRectF RuleArea::boundingRect() const
{
    return _boundingRect;
}

void RuleArea::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen p;
    p.setWidth(0);
    painter->setPen(p);

    painter->drawRect(_boundingRect);

    // de WorkArea TEST
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
    // end TEST
}

void RuleArea::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // para view drag
    event->ignore();
}

void RuleArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // para view drag
    event->ignore();
}

void RuleArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // para view drag
    event->ignore();
}
