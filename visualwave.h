#ifndef VISUALWAVE_H
#define VISUALWAVE_H

#include "visualgroup.h"

class ControlPoint;

class VisualWave : public VisualGroup
{
    Q_OBJECT

public:
    VisualWave(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size);
    using VisualGroup::VisualGroup;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    qreal linlin(qreal value, qreal inMin, qreal inMax, qreal outMin, qreal outMax);
    void updatePathItems(const qreal &lod);
    int obtainPointNumber(const QPointF &point);
    void editPoint(const QPointF &point);

    QList<qreal> fakeData;
    QGraphicsPathItem waveShapeItem;
    QGraphicsPathItem controlPointsItem;
    qreal controlPointRadio = 5;
    bool pointSelected = false;
    int selectedPointNumber = -1;
    int visualStartPos = 0;
    int visualEndPos = 0;
};

#endif // VISUALWAVE_H
