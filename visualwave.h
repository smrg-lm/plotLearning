#ifndef VISUALWAVE_H
#define VISUALWAVE_H

#include "visualgroup.h"

class VisualElement;

class VisualWave : public VisualGroup
{
public:
    VisualWave(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size);
    using VisualGroup::VisualGroup;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

private:
    QList<qreal> fakeData;
    QList<VisualElement*> vElements;
};

#endif // VISUALWAVE_H
