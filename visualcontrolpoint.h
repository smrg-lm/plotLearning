#ifndef VISUALCONTROLPOINT_H
#define VISUALCONTROLPOINT_H

#include "visualelement.h"

class ControlPoint : public VisualElement
{
public:
    using VisualElement::VisualElement;

    QPainterPath shape() const override; // circle or square
    QPointF center() const; // qtcreator tooltip points to QTextStream::center()
    void setCenterPos(const QPointF &pos);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    bool isSelected() const { return selected; }
    void setSelected(bool value) { selected = value; }

private:
    bool selected;
};

#endif // VISUALCONTROLPOINT_H
