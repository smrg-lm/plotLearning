#ifndef RULEAREA_H
#define RULEAREA_H

#include <QGraphicsObject>

class RuleArea : public QGraphicsObject
{
    Q_OBJECT

public:
    RuleArea(QGraphicsItem *parent = 0);

    QRectF rect() const;
    void setRect(const QRectF &rect);
    QRectF boundingRect() const; Q_DECL_OVERRIDE
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0); Q_DECL_OVERRIDE

private:
    QRectF _boundingRect;
};

#endif // RULEAREA_H
