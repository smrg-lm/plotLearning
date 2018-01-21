#include "visualobject.h"
#include "visualgroup.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

#include <QDebug>

VisualObject::VisualObject(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    this->setFlag(QGraphicsItem::ItemIsMovable, true); // TEST
}

VisualObject::VisualObject(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size)
    : QGraphicsItem(parent), _boundingRect(QRectF(QPointF(0, 0), size))
{
    this->setPos(pos);
    this->setFlag(QGraphicsItem::ItemIsMovable, true); // TEST
}

QRectF VisualObject::rect() const
{
    return _boundingRect;
}

void VisualObject::setRect(const QRectF &rect)
{
    if(rect != _boundingRect) {
        this->prepareGeometryChange();
        _boundingRect = rect;
    }
}

QRectF VisualObject::boundingRect() const
{
    return _boundingRect;
}

void VisualObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(Qt::gray);
    QPen p;
    p.setWidth(0);
    painter->setPen(p);

    painter->drawRect(_boundingRect);
}

void VisualObject::clipPosToParent()
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

void VisualObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void VisualObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

void VisualObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
     this->clipPosToParent();
}

void VisualObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void VisualObject::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QGraphicsItem::wheelEvent(event);
}
