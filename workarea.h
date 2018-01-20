#ifndef WORKAREA_H
#define WORKAREA_H

#include <QGraphicsObject>

class WorkArea : public QGraphicsObject
{
public:
    WorkArea(QGraphicsItem *parent = 0);

    QRectF rect() const;
    void setRect(const QRectF &rect);
    QRectF boundingRect() const; Q_DECL_OVERRIDE
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0); Q_DECL_OVERRIDE

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) {} Q_DECL_OVERRIDE
    void mousePressEvent(QGraphicsSceneMouseEvent*) {} Q_DECL_OVERRIDE
    void mouseMoveEvent(QGraphicsSceneMouseEvent*) {} Q_DECL_OVERRIDE
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) {} Q_DECL_OVERRIDE

private:
    QRectF _boundingRect;
};

#endif // WORKAREA_H
