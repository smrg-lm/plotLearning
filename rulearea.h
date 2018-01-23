#ifndef RULEAREA_H
#define RULEAREA_H

#include "visualgroup.h"

class RuleArea : public VisualGroup
{
    Q_OBJECT

public:
    RuleArea(QGraphicsItem *parent = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // RULEAREA_H
