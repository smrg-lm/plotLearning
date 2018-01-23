#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include "visualelement.h"

class ControlPoint : public VisualElement
{
public:
    using VisualElement::VisualElement;

    QPainterPath shape() const override;
    QPointF center() const { return this->boundingRect().center(); } // QTextStream??? UAT???
    void setCenterPos(const QPointF &pos) {
        this->setPos(pos - this->mapToParent(this->center())); } // genera un loop?

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    bool isSelected() const { return selected; }
    void setSelected(bool value) { selected = value; }

private:
    bool selected;
};

#endif // CONTROLPOINT_H