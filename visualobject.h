#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include <QGraphicsItem>

class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;

class VisualObject : public QGraphicsItem
{
public:
    VisualObject(QGraphicsItem *parent);
    VisualObject(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size); // es const&?

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

#endif // VISUALOBJECT_H
