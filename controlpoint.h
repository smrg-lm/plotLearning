#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include "visualelement.h"

class ControlPoint : public VisualElement
{
public:
    using VisualElement::VisualElement;

    QPainterPath shape() const override;
    QPointF center() const; // qtcreator tooltip points to QTextStream::center()
    void setCenterPos(const QPointF &pos);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    bool isSelected() const { return selected; }
    void setSelected(bool value) { selected = value; }

private:
    bool selected;
};

#endif // CONTROLPOINT_H
