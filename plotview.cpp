#include "plotview.h"
#include "workarea.h"

#include <QMouseEvent>
#include <QWheelEvent>

#include <QDebug>

PlotView::PlotView(QWidget *parent)
    :BaseView(parent)
{
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
    return QRectF(QPointF(), _workArea->size());
}

void PlotView::mouseDoubleClickEvent(QMouseEvent *event)
{
    BaseView::mouseDoubleClickEvent(event);
}

void PlotView::mousePressEvent(QMouseEvent *event)
{
    BaseView::mousePressEvent(event);
}

void PlotView::mouseMoveEvent(QMouseEvent *event)
{
    BaseView::mouseMoveEvent(event);
}

void PlotView::mouseReleaseEvent(QMouseEvent *event)
{
    BaseView::mouseReleaseEvent(event);
}

void PlotView::wheelEvent(QWheelEvent *event)
{
    qDebug() << "PlotView wheelEvent";
    event->ignore();
    BaseView::wheelEvent(event);
}
