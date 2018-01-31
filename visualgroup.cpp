#include "visualgroup.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

#include <QDebug>

VisualGroup::VisualGroup(QGraphicsItem *parent)
    : VisualGroup(parent, QPointF(), QSizeF())
{}

VisualGroup::VisualGroup(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size)
    : QGraphicsObject(parent), _boundingRect(QRectF(QPointF(0, 0), size))
{
    this->setPos(pos);
    this->setFlag(QGraphicsItem::ItemIsMovable, true); // TEST
}

QSizeF VisualGroup::size() const
{
    return _boundingRect.size();
}

void VisualGroup::setSize(const QSizeF &size)
{
    if(size != _boundingRect.size()) {
        this->prepareGeometryChange();
        _boundingRect.setSize(size);
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

QGraphicsView *VisualGroup::getCurrentActiveView() const
{
    QList<QGraphicsView*> views = this->scene()->views();

    for(int i = 0; i < views.size(); i++) {
        if(views[i]->hasFocus()) return views[i];
        // scrollviews *must* be sibilings of the view
        QObjectList sibilings = views[i]->parentWidget()->children();
        for(int j = 0; j < sibilings.size(); j++) {
            QWidget *child = dynamic_cast<QWidget*>(sibilings[j]);
            //qDebug() << "Child[j]: " << child << j;
            if(child && child->hasFocus()) return views[i];
        }
    }

    return 0;
}

QRectF VisualGroup::visibleRect() const
{
    QGraphicsView *view = this->getCurrentActiveView();
    if(!view) return QRectF();

    QPointF viewSize(
                view->viewport()->size().width(),
                view->viewport()->size().height());

    QPointF tl = this->deviceTransform(view->viewportTransform())
            .inverted().map(QPointF(0, 0));
    QPointF br = this->deviceTransform(view->viewportTransform())
            .inverted().map(viewSize);

    // intersección de la vista con el item en coordenadas locales
    return _boundingRect.intersected(QRectF(tl, br));
}

void VisualGroup::clipPosToParent() // sync!!! tengo que unificarlas
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

qreal VisualGroup::floorQuant(qreal value, qreal quant)
{
    // may not be exactly sc_floor
    return quant == 0. ? value : std::floor(value / quant) * quant;
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
