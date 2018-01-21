#include "abstractview.h"

#include <QResizeEvent>
#include <QScrollBar>

AbstractView::AbstractView(QWidget *parent)
    :QGraphicsView(parent), _zoomOnResize(false)
{
    this->setContentsMargins(0, 0, 0, 0);
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop); // Qt:AlignCenter también funciona
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void AbstractView::resizeEvent(QResizeEvent *event) // TEST
{
    if(_zoomOnResize && this->isActiveWindow()) { // isActiveWindow puede ser un problema también
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
