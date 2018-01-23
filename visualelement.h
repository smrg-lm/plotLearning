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
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

    void clipPosToParent(); // no sync con vg

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;

private:
    QRectF _boundingRect;
};

#endif // VISUALELEMENT_H
