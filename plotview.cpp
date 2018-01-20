#include "plotview.h"
#include "workarea.h"

#include <QMouseEvent>
#include <QWheelEvent>

PlotView::PlotView(QWidget *parent)
    :AbstractView(parent)
{
    this->setContentsMargins(0, 0, 0, 0);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _scene = new QGraphicsScene(this); // ownership
    this->setScene(_scene);

    // basic top level scene/visual element (QGraphicsView::sceneRect)
    this->setWorkArea(new WorkArea());
    _scene->setSceneRect(this->waRect());
}

WorkArea *PlotView::workArea() const
{
    return _workArea;
}

void PlotView::setWorkArea(WorkArea *workArea)
{
    // check nil, remove from scene, destroy? make private?
    _workArea = workArea;
    _scene->addItem(_workArea);
}

QRectF PlotView::waRect() const
{
    return _workArea->rect();
}

void PlotView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);
}

void PlotView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
}

void PlotView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void PlotView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void PlotView::wheelEvent(QWheelEvent *event)
{
    //QGraphicsView::wheelEvent(event); // implementa wheel scroll
    event->ignore();
}
