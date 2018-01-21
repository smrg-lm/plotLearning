#ifndef VISUALELEMENT_H
#define VISUALELEMENT_H

#include <QGraphicsItem>

class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;

class VisualElement : public QGraphicsItem
{
public:
    VisualElement(QGraphicsItem *parent);
    VisualElement(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size);

    QSizeF size() const;
    void setSize(const QSizeF &rect);
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

#endif // VISUALELEMENT_H
