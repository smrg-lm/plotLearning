#include "controlpoint.h"

#include <QPainter>

QPainterPath ControlPoint::shape() const
{
    // guardar en un miembro
    QPainterPath path;
    path.addEllipse(this->boundingRect());
    return path;
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
