#include "workarea.h"

#include <QPainter>

// TEST
#include "visualgroup.h"
#include "visualobject.h"
#include <QDebug>

WorkArea::WorkArea(QGraphicsItem *parent)
    : QGraphicsObject(parent)
{
    this->setRect(QRectF(0, 0, 500, 500)); // TEST
    // begin TEST
    VisualGroup *vg = new VisualGroup(this, QPointF(0, 0), QSizeF(200, 100));
    new VisualObject(vg, QPointF(0, 0), QSizeF(25, 25));
    new VisualObject(vg, QPointF(25, 25), QSizeF(25, 25));
    new VisualObject(vg, QPointF(50, 50), QSizeF(25, 25));
    new VisualObject(vg, QPointF(75, 75), QSizeF(25, 25));
    qDebug() << "children: " << this->childItems();
    qDebug() << "children: " << vg->childItems();
    qDebug() << "vg visible: " << vg->isVisible();
    // end TEST
}

QRectF WorkArea::rect() const
{
    return _boundingRect;
}

void WorkArea::setRect(const QRectF &rect)
{
    if(rect != _boundingRect) {
        this->prepareGeometryChange();
        _boundingRect = rect;
    }
}

QRectF WorkArea::boundingRect() const
{
    return _boundingRect;
}

void WorkArea::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen p;
    p.setWidth(0);
    painter->setPen(p);

    painter->drawRect(_boundingRect);

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
}
