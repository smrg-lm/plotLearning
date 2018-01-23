#include "visualwave.h"
#include "visualelement.h"

#include <QPainter>

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

    // cached elements
    for(int i; i < 10; i++) {
        vElements.append(new VisualElement(this));
    }
}

void VisualWave::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    Q_UNUSED(painter);

    // por ahora, el largo del widget vw es igual a la cantidad de muestras
    QRectF vr = this->visibleRect();
    // si no es visible *return* (¿antes o después de drawRect? ¿qué hace QGraphicsView?)
    // si no cambió la visibilidad *return*
    int startPos = (int)round(vr.left());
    int endPos = startPos + (int)round(vr.width());

    qDebug() << "list size: " << fakeData.size();
    qDebug() << "start: " << startPos;
    qDebug() << "end: " << endPos;
    qDebug() << "loop size: " << endPos - startPos;

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

        painter->drawLine(x1, y1, x2, y2); // hay artefactos al hacer srcoll de gv con scrollview
    }
}
