#include "workarea.h"

#include <QPainter>

// TEST
#include "visualgroup.h"
#include "visualelement.h"
#include "visualwave.h"
#include <QDebug>

WorkArea::WorkArea(QGraphicsItem *parent)
    : VisualGroup(parent)
{
    this->setSize(QSizeF(500, 500)); // TEST

    VisualGroup *vg = new VisualGroup(this, QPointF(0, 0), QSizeF(200, 100));
    new VisualElement(vg, QPointF(0, 0), QSizeF(25, 25));
    new VisualElement(vg, QPointF(25, 25), QSizeF(25, 25));
    new VisualElement(vg, QPointF(50, 50), QSizeF(25, 25));
    new VisualElement(vg, QPointF(75, 75), QSizeF(25, 25));
    qDebug() << "children: " << this->childItems();
    qDebug() << "children: " << vg->childItems();
    qDebug() << "vg visible: " << vg->isVisible();

    for(int i = 0; i < 25; i++) { // excede apropósito
        mList.append(new VisualElement(this, QPoint(i * 25, i * 25), QSize(25, 25)));
    }

    new VisualWave(this, QPointF(0, 0), QSizeF(300, 100));
}

void WorkArea::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen p;
    p.setWidth(0);
    painter->setPen(p);

    qreal x = 0;
    qreal y = 0;
    qreal width = this->size().width();
    qreal height = this->size().height();

    painter->drawRect(QRectF(x, y, width, height));

    for(int i = x; i < width + x; i += 25) {
        painter->drawLine(i, y, i, height + y);
    }

    for(int i = y; i < height + y; i += 25) {
        painter->drawLine(x, i, width + x, i);
    }
}
