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

private:
    QList<qreal> fakeData;
    QList<ControlPoint*> vElements;
};

#endif // VISUALWAVE_H
