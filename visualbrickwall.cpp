#include "visualbrickwall.h"

#include <QGraphicsSceneMouseEvent>

#include <QDebug>

// *** Brick ***

void VisualBrick::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = this->mapToParent(event->pos());

    // !!! entonces, en todo caso, el constructor no podría aceptar otra clase parent
    // !!! que no fuera VisualBrickWall, y cualquier subclase puede tener problemas
    VisualBrickWall *parent = static_cast<VisualBrickWall*>(this->parentItem());
    if(!parent) return;

    QPointF rp = parent->snapPoint(pos);
    mouseDragStartRect = QRectF(rp.x(), rp.y(), parent->timeQuant(), parent->resolution());
    mouseDragOffset = rp - this->pos();
}

void VisualBrick::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = this->mapToParent(event->pos());

    if(!mouseDragStartRect.contains(pos)) {
        VisualBrickWall *parent = static_cast<VisualBrickWall*>(this->parentItem());
        if(!parent) return;

        this->setPos(parent->snapPoint(pos - mouseDragOffset));
        this->clipPosToParent();
        _start = this->x(); // QGraphicsItem::data?

        QPointF rp = parent->snapPoint(pos);
        mouseDragStartRect = QRectF(rp.x(), rp.y(), parent->timeQuant(), parent->resolution());
    }
}


// *** Wall ***

VisualBrickWall::VisualBrickWall(qreal range, qreal resolution)
    : VisualBrickWall(nullptr, range, resolution) {}

VisualBrickWall::VisualBrickWall(QGraphicsItem *parent, qreal range, qreal resolution)
    : VisualGroup(parent)
{
    this->setRange(range);
    this->setResolution(resolution);
    this->setDur(30); // default
}

// note: height es independiente de range
qreal VisualBrickWall::range() const
{
    if(_range < 0) return this->size().height();
    else return _range;
}

// note: height es independiente de range
void VisualBrickWall::setRange(qreal value)
{
    _range = value;
    if(_range >= 0)
        this->setSize(QSizeF(this->size().width(), _range));
}

qreal VisualBrickWall::resolution() const
{
    return _resolution;
}

void VisualBrickWall::setResolution(qreal value)
{
    if(value > _range) _resolution = _range;
    else if(value <= 0) _resolution = 0;
    else _resolution = value;
}

qreal VisualBrickWall::dur() const
{
    return _dur;
}

// width es dependiente de dur (es lo mismo como unidades gráficas)
// no se puede usar parent::setSize directamente
void VisualBrickWall::setDur(qreal value)
{
    if(value < 0) _dur = 0;
    else _dur = value;
    this->setSize(QSize(_dur, this->size().height()));
}

qreal VisualBrickWall::timeQuant() const
{
    return _timeQuant;
}

void VisualBrickWall::setTimeQuant(qreal value)
{
    if(value < 0) _timeQuant = 0;
    else _timeQuant = value;
}

qreal VisualBrickWall::snapRound(qreal value, qreal quant)
{
    // sc_round inlinebynaryop
    return quant == 0. ? value : std::floor(value/quant + .5) * quant;
}

qreal VisualBrickWall::snapFloor(qreal value, qreal quant)
{
    // may not be exactly sc_floor
    return quant == 0. ? value : std::floor(value / quant) * quant;
}

qreal VisualBrickWall::verticalSnap(qreal value)
{
    //return this->sc_round(value, _resolution);
    return this->snapFloor(value, _resolution);
}

qreal VisualBrickWall::horizontalSnap(qreal value)
{
    //return this->sc_round(value, _timeQuant);
    return this->snapFloor(value, _timeQuant);
}

QPointF VisualBrickWall::snapPoint(const QPointF &point)
{
    QPointF rp;

    if(this->timeQuant() > 0)
        rp.setX(this->horizontalSnap(point.x()));
    else
        rp.setX(point.x());

    if(this->resolution() > 0)
        rp.setY(this->verticalSnap(point.y()));
    else
        rp.setY(point.y());

    return rp;
}
