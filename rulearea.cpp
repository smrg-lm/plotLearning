#include "rulearea.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

RuleArea::RuleArea(QGraphicsItem *parent)
    : VisualGroup(parent)
{}

void RuleArea::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen p;
    p.setWidth(0);
    painter->setPen(p);

    painter->drawRect(QRectF(QPointF(), this->size()));

    // de WorkArea TEST
    qreal x = 0;
    qreal y = 0;
    qreal width = this->size().width();
    qreal height = this->size().height();

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
