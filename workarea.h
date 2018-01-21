#ifndef WORKAREA_H
#define WORKAREA_H

#include "visualgroup.h"

class WorkArea : public VisualGroup
{
    Q_OBJECT

public:
    WorkArea(QGraphicsItem *parent = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0); Q_DECL_OVERRIDE
};

#endif // WORKAREA_H
