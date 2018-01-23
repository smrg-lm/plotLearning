#ifndef WORKAREA_H
#define WORKAREA_H

#include "visualgroup.h"
#include "visualelement.h"

class WorkArea : public VisualGroup
{
    Q_OBJECT

public:
    WorkArea(QGraphicsItem *parent = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

private:
    QList<VisualElement *> mList;
};

#endif // WORKAREA_H
