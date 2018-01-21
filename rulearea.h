#ifndef RULEAREA_H
#define RULEAREA_H

#include "visualgroup.h"

class RuleArea : public VisualGroup
{
    Q_OBJECT

public:
    RuleArea(QGraphicsItem *parent = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0); Q_DECL_OVERRIDE

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event); Q_DECL_OVERRIDE
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event); Q_DECL_OVERRIDE
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event); Q_DECL_OVERRIDE
};

#endif // RULEAREA_H
