#include "abstractview.h"

#include <QResizeEvent>
#include <QScrollBar>
#include <QWheelEvent>

#include <QDebug>

AbstractView::AbstractView(QWidget *parent)
    :QGraphicsView(parent), _zoomOnResize(false)
{
    this->setContentsMargins(0, 0, 0, 0);
    //this->setAlignment(Qt::AlignLeft | Qt::AlignTop); // Qt:AlignCenter también funciona
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

void AbstractView::wheelEvent(QWheelEvent *event)
{
    qDebug() << "AbstractView wheelEvent: " << event;

    bool shift = event->modifiers() & Qt::ShiftModifier;
    bool ctrl = event->modifiers() & Qt::ControlModifier;
    qreal ad = 1 + (qreal)event->angleDelta().y() / (360 * 5); // ~0.06%

    qDebug() << "ad: " << ad;

    if(shift && ctrl) {
        this->scale(ad, ad);
    } else if(shift) {
        this->scale(ad, 1);
    } else if(ctrl) {
        this->scale(1, ad);
    }

    event->accept();
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
