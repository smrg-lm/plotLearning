#ifndef VISUALBRICKWALL_H
#define VISUALBRICKWALL_H

#include "visualelement.h"
#include "visualgroup.h"

class VisualBrick : public VisualElement
{
public:
    using VisualElement::VisualElement;

    //void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    //void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    // pueden ser el mismo valor que x/y?
    qreal _value; // QGraphicsItem::data?
    qreal _start; // QGraphicsItem::data? if start is pos().x()
    qreal _dur; // QGraphicsItem::data? if dur is size().x()

    QRectF mouseDragStartRect;
    QPointF mouseDragOffset;
};

// non interleaved, bad built, wall :)
class VisualBrickWall : public VisualGroup
{
    Q_OBJECT

public:
    using VisualGroup::VisualGroup;
    VisualBrickWall(qreal range, qreal resolution);
    VisualBrickWall(QGraphicsItem *parent, qreal range, qreal resolution);

    qreal range() const;
    void setRange(qreal value);
    qreal resolution() const;
    void setResolution(qreal value);
    qreal dur() const;
    void setDur(qreal value);
    qreal timeQuant() const;
    void setTimeQuant(qreal value);

    // no need for layout but snap
    qreal snapRound(qreal value, qreal quant);
    qreal snapFloor(qreal value, qreal quant);
    qreal verticalSnap(qreal value);
    qreal horizontalSnap(qreal value);

private:
    qreal _range = -1; // abs: 0..inf
    qreal _resolution = 0;
    qreal _dur;
    qreal _timeQuant = 1;
};

#endif // VISUALBRICKWALL_H
