#include "visualelement.h"
#include "visualgroup.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

#include <QDebug>

VisualElement::VisualElement(QGraphicsItem *parent)
    : VisualElement(parent, QPointF(), QSizeF())
{}

VisualElement::VisualElement(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size)
    : QGraphicsItem(parent), _boundingRect(QRectF(QPointF(0, 0), size))
{
    this->setPos(pos);
    this->setFlag(QGraphicsItem::ItemIsMovable, true); // TEST
}

QSizeF VisualElement::size() const
{
    return _boundingRect.size();
}

void VisualElement::setSize(const QSizeF &size)
{
    if(size != _boundingRect.size()) {
        this->prepareGeometryChange();
        _boundingRect.setSize(size);
    }
}

QRectF VisualElement::boundingRect() const
{
    return _boundingRect;
}

void VisualElement::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
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

void VisualElement::clipPosToParent() // sync!!! tengo que unificarlas
{
    QGraphicsItem *parent = this->parentItem();
    if(!parent) return;

    QPointF pos = this->pos();
    QPointF newPos;

    if(!(pos.x() < 0))
        newPos.setX(pos.x());
    if(!(pos.y() < 0))
        newPos.setY(pos.y());

    if(_boundingRect.width() <= parent->boundingRect().width() &&
            pos.x() + _boundingRect.width() > parent->boundingRect().width())
        newPos.setX(parent->boundingRect().width() - _boundingRect.width());

    if(_boundingRect.height() <= parent->boundingRect().height() &&
            pos.y() + _boundingRect.height() > parent->boundingRect().height())
        newPos.setY(parent->boundingRect().height() - _boundingRect.height());

    this->setPos(newPos);
}

void VisualElement::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void VisualElement::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

void VisualElement::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
     this->clipPosToParent();
}

void VisualElement::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}


void VisualElement::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QGraphicsItem::wheelEvent(event);
}
