#ifndef VISUALGROUP_H
#define VISUALGROUP_H

#include <QGraphicsObject>

class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QGraphicsView;

class VisualGroup : public QGraphicsObject // debería tener raíz común con VisualObject/Element
{
    Q_OBJECT

public:
    VisualGroup(QGraphicsItem *parent);
    VisualGroup(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size);

    QSizeF size() const;
    void setSize(const QSizeF &size);
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

    QRectF visibleRect() const; // redo, usa view
    void clipPosToParent(); // qué hacer cuando el sub-elemento es más grande que parent

    QGraphicsView *getCurrentActiveView();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;

private:
    QRectF _boundingRect;
};

#endif // VISUALGROUP_H
