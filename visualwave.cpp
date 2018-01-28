#include "visualwave.h"
#include "visualcontrolpoint.h"

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
    controlPointsItem.setParentItem(this);
    controlPointsItem.setPen(pen);
    controlPointsItem.setBrush(Qt::gray);
    waveShapeItem.setParentItem(this);
    waveShapeItem.setPen(pen);
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
    if(controlPointsItem.contains(event->pos())) {
        selectedPointNumber = this->obtainPointNumber(event->pos());
        qDebug() << "selectePointNumber: " << selectedPointNumber;
        if(selectedPointNumber > 0) pointSelected = true;
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
        selectedPointNumber = -1;
        pointSelected = false;
    } else {
        VisualGroup::mouseReleaseEvent(event);
    }
}

qreal VisualWave::linlin(qreal value, qreal inMin, qreal inMax, qreal outMin, qreal outMax)
{
    // sc linlin: (this-inMin)/(inMax-inMin) * (outMax-outMin) + outMin;
    return (value - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
}

void VisualWave::updatePathItems(const qreal &lod)
{
    QRectF vr = this->visibleRect(); // se necesitaría mantener la última para cuando la vista no está activa, o ver si se puede cambiar la implementación
    visualStartPos = (int)round(vr.left());
    visualEndPos = visualStartPos + (int)round(vr.width());
    // ^^^^^^^^^ en algún caso, tal vez zoom extremo, se va de rango
    // -> ASSERT failure in QList<T>::operator[]: "index out of range"

    QPainterPath wavePath;
    QPainterPath controlsPath;

    // waveShapeItem
    qreal x = visualStartPos;
    qreal y = this->linlin(fakeData[visualStartPos], -1, 1, 0, this->boundingRect().height());
    wavePath.moveTo(x, y);

    // controlPointsItem
    qreal minLod = 6;
    qreal pointSizeX, pointSizeY;
    if(lod > minLod) {
        // counter transform por point size (QTransform::map documentation)
        QGraphicsView *view = this->getCurrentActiveView();
        if(!(view == 0)) {
            QTransform t;
            qreal ox, oy;

            t = this->deviceTransform(view->viewportTransform()).inverted();
            ox = oy = controlPointRadio; //controlPointRadio is untransformed

            pointSizeX = t.m11() * ox + t.m21() * oy;
            pointSizeY = t.m22() * oy + t.m12() * ox;
            if(!t.isAffine()) {
                qreal wp = t.m13() * ox + t.m23() * y + t.m33();
                pointSizeX /= wp;
                pointSizeY /= wp;
            }
        }
        controlsPath.addEllipse(QPointF(x, y), pointSizeX, pointSizeY);
    }

    // both
    for(int i = visualStartPos + 1; i < visualEndPos; i++) {
        x = i;
        y = this->linlin(fakeData[i], -1, 1, 0, this->boundingRect().height());
        wavePath.lineTo(x, y);
        if(lod > minLod) controlsPath.addEllipse(QPointF(x, y), pointSizeX, pointSizeY);
    }

    waveShapeItem.setPath(wavePath);
    controlPointsItem.setPath(controlsPath);
}

int VisualWave::obtainPointNumber(const QPointF &point)
{
    // las posiciones coinciden con las unidades del gráfico como números enteros
    QList<QPolygonF> points = controlPointsItem.path().toSubpathPolygons();

    for(int i = 0; i < points.size(); i++) {
        if(points[i].containsPoint(point, Qt::OddEvenFill)) return i;
    }

    // may fail depending on the polygon representation
    return -1;
}

void VisualWave::editPoint(const QPointF &point)
{
    // this is not cpu friendly (yet)
    if(point.y() > this->boundingRect().height() || point.y() < 0) return;
    qreal newValue = this->linlin(point.y(), 0, this->boundingRect().height(), -1, 1);
    if(newValue > 1) newValue = 1; if(newValue < -1) newValue = -1;
    // this needs some kind of range and resolution control
    // and enable tooltip showing values in rt
    fakeData[visualStartPos + selectedPointNumber] = this->linlin(point.y(), 0, this->boundingRect().height(), -1, 1);
    this->update();
}
