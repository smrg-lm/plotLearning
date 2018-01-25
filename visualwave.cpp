#include "visualwave.h"
#include "controlpoint.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include <QGraphicsView>
#include <QGraphicsScene>

#include <QDebug>


VisualWave::VisualWave(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size)
    : VisualGroup(parent, pos, size)
{
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(-1, 1);

    for(int i = 0; i < this->size().width(); i++) {
        fakeData.append(dist(e2));
    }

    // QGraphicsPathItem (QPainterPath)
    QPen pen; pen.setWidth(0);
    controlPoints.setParentItem(this);
    controlPoints.setPen(pen);
    controlPoints.setBrush(Qt::gray);
    pathItem.setParentItem(this);
    pathItem.setPen(pen);

    pointSelected = false;
}

void VisualWave::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(Qt::lightGray);
    QPen p;
    p.setWidth(0);
    painter->setPen(p);
    painter->drawRect(this->boundingRect());

    // pero lod no indica diferencia h/v
    // tal ve hay que usar QGraphicsItem::deviceTransform (ver doc)
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    this->updatePathItems(lod); // confirmar que actualiza correctamente
}

void VisualWave::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    VisualGroup::mouseDoubleClickEvent(event);
}

void VisualWave::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(controlPoints.contains(event->pos())) {
        pointSelected = true;
        //event->accept(); // por defecto acepta
    } else {
        VisualGroup::mousePressEvent(event); // se necesita para itemmove en los tres handlers
    }
}

void VisualWave::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(pointSelected) {
        this->editPoint(event->pos());
    } else {
        VisualGroup::mouseMoveEvent(event);
    }
}

void VisualWave::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(pointSelected) {
        pointSelected = false;
    } else {
        VisualGroup::mouseReleaseEvent(event);
    }
}

void VisualWave::updatePathItems(const qreal &lod)
{
    // linlin: (this-inMin)/(inMax-inMin) * (outMax-outMin) + outMin;
    auto linlin = [] (qreal value, qreal inMin, qreal inMax, qreal outMin, qreal outMax) {
        return (value - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
    };

    QRectF vr = this->visibleRect();
    int startPos = (int)round(vr.left());
    int endPos = startPos + (int)round(vr.width());
    //  ^^^^^^ en algún caso, tal vez zoom extremo, se va de rango
    // -> ASSERT failure in QList<T>::operator[]: "index out of range"

    QPainterPath path;
    QPainterPath controls;

    // pathItem
    qreal x = startPos;
    qreal y = linlin(fakeData[startPos], -1, 1, 0, this->boundingRect().height());
    path.moveTo(x, y);

    // controlPoints
    qreal minLod = 8;
    qreal pointSize = 5; // untransformed
    qreal pointSizeX, pointSizeY;
    if(lod > minLod) {
        // counter transform por  point size (QTransform::map documentation)
        QGraphicsView *view = this->getCurrentActiveView();
        if(!(view == 0)) {
            QTransform t;
            qreal ox, oy;

            t = this->deviceTransform(view->viewportTransform()).inverted();
            ox = oy = pointSize;

            pointSizeX = t.m11() * ox + t.m21() * oy;
            pointSizeY = t.m22() * oy + t.m12() * ox;
            if(!t.isAffine()) {
                qreal wp = t.m13() * ox + t.m23() * y + t.m33();
                pointSizeX /= wp;
                pointSizeY /= wp;
            }
        }
        controls.addEllipse(QPointF(x, y), pointSizeX, pointSizeY);
    }

    // both
    for(int i = startPos + 1; i < endPos; i++) {
        x = i;
        y = linlin(fakeData[i], -1, 1, 0, this->boundingRect().height());
        path.lineTo(x, y);
        if(lod > minLod) controls.addEllipse(QPointF(x, y), pointSizeX, pointSizeY);
    }

    pathItem.setPath(path);
    controlPoints.setPath(controls);
}

void VisualWave::editPoint(const QPointF &point)
{

}
