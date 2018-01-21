#ifndef VISUALGROUP_H
#define VISUALGROUP_H

#include <QGraphicsObject>

class VisualGroup : public QGraphicsObject
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

private:
    QRectF _boundingRect;
};

#endif // VISUALGROUP_H
