#include "workarea.h"

#include <QPainter>

// TEST
#include "visualgroup.h"
#include "visualelement.h"
#include "visualwave.h"
#include "visualbrickwall.h"
#include <QDebug>

WorkArea::WorkArea(QGraphicsItem *parent)
    : VisualGroup(parent)
{
    this->setSize(QSizeF(500, 200)); // TEST
    this->setFlag(QGraphicsItem::ItemIsMovable, false); // no se puede mover o hay que sincronizar con sceneRect

    // TEST
    new VisualWave(this, QPointF(0, 0));

    VisualBrickWall *brickWall = new VisualBrickWall(this, 50, 5); // default dur 30
    brickWall->setPos(0, 50);
    brickWall->setDur(100);
    for(int i = 0; i < 10; i++) {
        new VisualBrick(brickWall, QPoint(i * 2, i * 5), QSize(2, 5));
    }
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
