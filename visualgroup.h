#ifndef VISUALGROUP_H
#define VISUALGROUP_H

#include <QGraphicsObject>

class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;

class VisualGroup : public QGraphicsObject // debería tener raíz común con VisualObject/Element
{
    Q_OBJECT

public:
    VisualGroup(QGraphicsItem *parent);
    VisualGroup(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size); // es const&?

    QRectF rect() const;
    void setRect(const QRectF &rect);
    QRectF boundingRect() const; Q_DECL_OVERRIDE
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0); Q_DECL_OVERRIDE

    void clipPosToParent();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event); Q_DECL_OVERRIDE
    void mousePressEvent(QGraphicsSceneMouseEvent *event); Q_DECL_OVERRIDE
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event); Q_DECL_OVERRIDE
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event); Q_DECL_OVERRIDE
    void wheelEvent(QGraphicsSceneWheelEvent *event); Q_DECL_OVERRIDE

private:
    QRectF _boundingRect;
};

#endif // VISUALGROUP_H
