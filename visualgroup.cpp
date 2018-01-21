#include "visualgroup.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

#include <QDebug>

VisualGroup::VisualGroup(QGraphicsItem *parent)
    : QGraphicsObject(parent)
{
    this->setFlag(QGraphicsItem::ItemIsMovable, true); // TEST
}

VisualGroup::VisualGroup(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size)
    : QGraphicsObject(parent), _boundingRect(QRectF(QPointF(0, 0), size))
{
    this->setPos(pos);
    this->setFlag(QGraphicsItem::ItemIsMovable, true); // TEST
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

void VisualGroup::clipPosToParent()
{
    VisualGroup *parent = static_cast<VisualGroup*>(this->parentItem());
    if(parent == 0) return;

    QPointF pos = this->pos();
    QPointF newPos;

    if(!(pos.x() < 0))
        newPos.setX(pos.x());
    if(!(pos.y() < 0))
        newPos.setY(pos.y());
    if(pos.x() + _boundingRect.width() > parent->boundingRect().width())
        newPos.setX(parent->boundingRect().width() - _boundingRect.width());
    if(pos.y() + _boundingRect.height() > parent->boundingRect().height())
        newPos.setY(parent->boundingRect().height() - _boundingRect.height());

    this->setPos(newPos);
}

void VisualGroup::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseDoubleClickEvent(event);
}

void VisualGroup::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mousePressEvent(event);
}

void VisualGroup::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseMoveEvent(event);
    this->clipPosToParent();
}

void VisualGroup::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseReleaseEvent(event);
}

void VisualGroup::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QGraphicsObject::wheelEvent(event);
}
