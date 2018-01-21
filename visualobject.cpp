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
    QPointF pos = this->pos();
    QPointF newPos;

    qDebug() << "this pos: " << pos;
    qDebug() << "this boundingRect: " << _boundingRect;
    qDebug() << "parent: " << parent;
    qDebug() << "parent boundingRect: " << parent->boundingRect();
    qDebug() << "mapToParent: " << this->mapToParent(pos); // ???

    //this->setPos(newPos);
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
