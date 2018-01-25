#include "visualwave.h"
#include "controlpoint.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

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

    // cached elements, aparecen según LOD (isVisible) y visibleRect
    // ver GraphicsScene::minimumRenderSize, tal vez no sea necesario
    for(int i; i < this->size().width() / 4; i++) {
        ControlPoint *cp = new ControlPoint(this, QPointF(), QSizeF(10, 10));
        cp->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        vElements.append(cp);
    }
}

void VisualWave::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    /*
     * QGraphicsItem::paint()
     * Note: it is mandatory that an item will always redraw itself in the exact
     * same way, unless update() was called; otherwise visual artifacts may occur.
     * In other words, two subsequent calls to paint() must always produce the same
     * output, unless update() was called between them.
     *
     * No se pueden cambiar los estados en el método paint(), menos aún los que
     * llaman a update()
     */

    // por ahora, el largo del widget vw es igual a la cantidad de muestras
    QRectF vr = this->visibleRect();
    int startPos = (int)round(vr.left());
    int endPos = startPos + (int)round(vr.width());

    // worldTransform es == a QGraphicsView::transform() o viewportTransform() ?
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    qDebug() << "LOD: " << lod;

    qDebug() << "worldTransform: " << painter->worldTransform();
    qDebug() << "sceneasdfasdff: " << this->scene()->views()[0]->transform();
    qDebug() << "tfasdfasdfasdf: " << (this->scene()->views()[0]->transform() == painter->worldTransform());

    // linlin
    //(this-inMin)/(inMax-inMin) * (outMax-outMin) + outMin;
    auto linlin = [] (qreal value, qreal inMin, qreal inMax, qreal outMin, qreal outMax) {
        return (value - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
    };

    painter->setBrush(Qt::lightGray);
    QPen p;
    p.setWidth(0);
    painter->setPen(p);
    painter->drawRect(this->boundingRect());

    int prevPos = startPos;
    for(int i = startPos + 1; i < endPos; i++) {
        qreal x1 = prevPos;
        qreal y1 = linlin(fakeData[prevPos], -1, 1, 0, this->boundingRect().height());
        qreal x2 = i;
        qreal y2 = linlin(fakeData[i], -1, 1, 0, this->boundingRect().height());
        prevPos = i;
        painter->drawLine(x1, y1, x2, y2); // *** hay artefactos al hacer srcoll de gv con scrollview lento (puede ser que no actualiza?)
    }

    //  no se puede setear estas propiedades de los items dentro de paint
    if(lod > 4) { // totalmente a ojo/azar
        for(int i = 0; i < vElements.size(); i++) {
            int dataPos = startPos + i;
            if(dataPos >= fakeData.size()) break;
            qreal y = linlin(fakeData[dataPos], -1, 1, 0, this->boundingRect().height());
            // así no se pueden mover, mal, tal vez se puede checkar
            // que no haya cambiado visibleRect, tal vez se puede hacer
            // de otra manera totalmente distinta mejor
            vElements[i]->setPos(dataPos, y);
            //vElements[i]->setCenterPos(QPointF(dataPos, y));
            vElements[i]->setVisible(true);
        }
    } else {
        for(int i = 0; i < vElements.size(); i++) {
            vElements[i]->setVisible(false); // hacer un flag para el if, choca con visibleRect:return
        }
    }
}
