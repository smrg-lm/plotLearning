#include "scrollarea.h"

#include <QPainter>
#include <QMouseEvent>

#include <QDebug>

ScrollArea::ScrollArea(QWidget *parent)
    : QWidget(parent)
{
    _sizeHint = QSize(25, 25);
    this->setMinimumSize(_sizeHint);
    this->setOrientation(Qt::Horizontal);

    _scaleX = _scaleY = 1.0;
    _value = 0;
    _range = 0;
}

void ScrollArea::paintEvent(QPaintEvent *event)
{
    // LA OTRA ES HACER QUE ESTE WIDGET SEA UN GRAPHICS VIEW
    // Y NI SIQUIERA TENGO QUE IMPLMENTAR LOS COMPORTAMIENTOS
    // SOLO LAS CONEXIONES... SERÍA LO MÁS LÓGICO... NO, LUCAS?
    Q_UNUSED(event);
    QPainter painter(this);

    QPen p;
    p.setCosmetic(true);

    int widgetWidth = this->width();
    int widgetHeight = this->height();
    qreal penWidth = painter.pen().width();
    p.setColor(QColor(Qt::lightGray));
    painter.setPen(p);
    painter.setBrush(QColor(Qt::white));
    painter.drawRect(-penWidth / 2, -penWidth / 2,
                     widgetWidth - penWidth / 2, widgetHeight - penWidth / 2);

    //painter.setTransform(QTransform().scale(_scaleX, _scaleY));
    p.setColor(QColor(Qt::black));
    painter.setPen(p);
    int spacing = 25;
    if(this->orientation() == Qt::Horizontal) {
        //QPen pen; pen.setWidth(0); painter.setPen(pen); // casi... TEST
        painter.setTransform(QTransform().scale(_scaleX, 1)); // casi... TEST
        for(int i = _value % spacing; i < widgetWidth; i += spacing) { // value TEST
            int x = i + 1; // ver este offset
            painter.drawLine(x, 0, x, widgetHeight);
        }
    } else {
        //QPen pen; pen.setWidth(0); painter.setPen(pen); // casi... TEST
        painter.setTransform(QTransform().scale(1, _scaleY)); // casi... TEST
        for(int i = _value % spacing; i < widgetHeight; i += spacing) { // value TEST
            int y = i + 1; // ver este offset
            painter.drawLine(0, y, widgetWidth, y);
        }
    }
}

void ScrollArea::setOrientation(Qt::Orientation o)
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

void ScrollArea::mouseDoubleClickEvent(QMouseEvent *event)
{
    QWidget::mouseDoubleClickEvent(event);
}

void ScrollArea::mousePressEvent(QMouseEvent *event)
{
    if(_orientation == Qt::Horizontal) {
        _ipos = event->globalX();
    } else {
        _ipos = event->globalY();
    }
}

void ScrollArea::mouseMoveEvent(QMouseEvent *event)
{
    int pos, dpos, auxValue, length;

    if(_orientation == Qt::Horizontal) {
        length = this->width();
        pos = event->globalX();
    } else {
        length = this->height();
        pos = event->globalY();
    }

    dpos = pos - _ipos;
    _ipos = pos;
    auxValue = _value + dpos;

    if(auxValue <= 0 && auxValue >= length /*-2*/ - _range) { // no muestra/dibuja, falta a la derecha/abajo (2px, qué es?), falta sync on resize
        _value += dpos;
        this->update();
        emit deltaScrollChanged(dpos);
//        qDebug() << "width = " << this->width();
//        qDebug() << "range: " << _range;
//        qDebug() << "delta: " << dpos;
//        qDebug() << "value: " << _value;
    }
}

void ScrollArea::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}
