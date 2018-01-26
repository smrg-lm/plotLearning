#include "visualcontrolpoint.h"

#include <QPainter>

QPainterPath ControlPoint::shape() const
{
    // guardar en un miembro
    QPainterPath path;
    path.addEllipse(this->boundingRect());
    return path;
}

QPointF ControlPoint::center() const
{
    return this->boundingRect().center();
}

void ControlPoint::setCenterPos(const QPointF &pos)
{
    this->setPos(pos - this->mapToParent(this->center()));
}

void ControlPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(Qt::green);
    QPen p;
    p.setWidth(0);
    painter->setPen(p);

    painter->drawEllipse(this->boundingRect());
}
