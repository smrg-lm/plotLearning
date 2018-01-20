#include "abstractview.h"

#include <QResizeEvent>
#include <QScrollBar>

AbstractView::AbstractView(QWidget *parent)
    :QGraphicsView(parent), _zoomOnResize(false)
{
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop); // No Qt:AlignCenter
}

void AbstractView::resizeEvent(QResizeEvent *event) // TEST
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

void AbstractView::horizontalScrollByDelta(int d)
{
    this->horizontalScrollBar()->setValue(
                this->horizontalScrollBar()->value() - d);
}

void AbstractView::verticalScrollByDelta(int d)
{
    this->verticalScrollBar()->setValue(
                this->verticalScrollBar()->value() - d);
}
