#include "scrollview.h"
#include "rulearea.h"

#include <QMouseEvent>
#include <QWheelEvent>

ScrollView::ScrollView(QWidget *parent)
    : AbstractView(parent)
{
    this->setOrientation(Qt::Horizontal);

    // como no puede crecer se mantiene pero shake it (parece una cubetera)
    this->setDragMode(QGraphicsView::ScrollHandDrag);

    _scene = new QGraphicsScene(this); // ownership
    this->setScene(_scene);

    // basic top level scene/visual element (QGraphicsView::sceneRect)
    this->setRuleArea(new RuleArea());
    //_scene->setSceneRect(this->waRect());
}

void ScrollView::setOrientation(Qt::Orientation o)
{
    _orientation = o;
    if(_orientation == Qt::Horizontal) {
        this->setSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Preferred);
    } else {
        this->setSizePolicy(
                    QSizePolicy::Preferred,
                    QSizePolicy::Ignored);
    }
}

RuleArea *ScrollView::ruleArea() const
{
    return _ruleArea;
}

void ScrollView::setRuleArea(RuleArea *ruleArea)
{
    // check nil, remove from scene, destroy? make private?
    _ruleArea = ruleArea;
    _scene->addItem(_ruleArea);
}

void ScrollView::updateScale(qreal sx, qreal sy)
{
    if(_orientation == Qt::Horizontal) {
        QGraphicsView::scale(sx, 1);
    } else {
        QGraphicsView::scale(1, sy);
    }
}

void ScrollView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);
}

void ScrollView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
}

void ScrollView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void ScrollView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void ScrollView::wheelEvent(QWheelEvent *event)
{
    //QGraphicsView::wheelEvent(event); // implementa wheel scroll
    event->ignore();
}
