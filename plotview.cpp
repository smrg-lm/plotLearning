#include "plotview.h"
#include "workarea.h"

#include <QMouseEvent>
#include <QResizeEvent>
#include <QScrollBar>

#include <QDebug>

PlotView::PlotView(QWidget *parent)
    :QGraphicsView(parent)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _scene = new QGraphicsScene(this); // ownership, ver luego con más vistas
    this->setScene(_scene); // setScene doesn't take ownership of the scene

    // basic top level scene/visual element
    // como la propiedad QGraphicsView::sceneRect
    // ver luego el diseño
    this->setWorkArea(new WorkArea());
    _scene->setSceneRect(this->waRect());

    this->setAlignment(Qt::AlignLeft | Qt::AlignTop); // No Qt:AlignCenter
    _zoomOnResize = false;
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

void PlotView::resizeEvent(QResizeEvent *event) // TEST
{
    if(_zoomOnResize && this->isActiveWindow()) {
        qreal sx = (qreal)event->size().width() /
                (qreal)event->oldSize().width();
        qreal sy = (qreal)event->size().height() /
                (qreal)event->oldSize().height();
        this->scale(sx, sy);
    }

    event->ignore();
    QGraphicsView::resizeEvent(event);
}

void PlotView::horizontalScrollByDelta(int d)
{
    this->horizontalScrollBar()->setValue(
                this->horizontalScrollBar()->value() - d);
}

void PlotView::verticalScrollByDelta(int d)
{
    this->verticalScrollBar()->setValue(
                this->verticalScrollBar()->value() - d);
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
