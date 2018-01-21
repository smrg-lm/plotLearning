#include "plotwidget.h"
#include "plotview.h"
#include "scrollview.h"
#include "rulearea.h"

#include <QGridLayout>
#include <QScrollBar>

#include <QDebug>

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setContentsMargins(10, 10, 10, 10);

    _layout = new QGridLayout(this);
    _layout->setSpacing(0);
    _layout->setMargin(0);
    _layout->setContentsMargins(0, 0, 0, 0);

    _plot = new PlotView(this);
    _layout->addWidget(_plot, 1, 0);

    //_plot->scene()->addItem(new QGraphicsRectItem()); // TEST

    // sync horizontal scroll
    _horizontalScrollView = new ScrollView();
    _horizontalScrollView->ruleArea()->setRect(QRectF(0, 0, 500, 25));
    _layout->addWidget(_horizontalScrollView, 0, 0);

    connect(_horizontalScrollView->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            _plot->horizontalScrollBar(), SLOT(setValue(int)));
    connect(_plot->horizontalScrollBar(), SIGNAL(valueChanged(int)),
             _horizontalScrollView->horizontalScrollBar(), SLOT(setValue(int)));

    // sync vertical scroll
    _verticalScrollView = new ScrollView();
    _verticalScrollView->setOrientation(Qt::Vertical);
    _verticalScrollView->ruleArea()->setRect(QRectF(0, 0, 25, 500));
    _layout->addWidget(_verticalScrollView, 1, 1);

    connect(_verticalScrollView->verticalScrollBar(), SIGNAL(valueChanged(int)),
            _plot->verticalScrollBar(), SLOT(setValue(int)));
    connect(_plot->verticalScrollBar(), SIGNAL(valueChanged(int)),
            _verticalScrollView->verticalScrollBar(), SLOT(setValue(int)));

    // sync zoom
    _plot->setZoomOnResize(false); // TEST
    connect(_plot, SIGNAL(scaleChanged(qreal,qreal)),
            _horizontalScrollView, SLOT(updateScale(qreal,qreal)));
    connect(_plot, SIGNAL(scaleChanged(qreal,qreal)),
            _verticalScrollView, SLOT(updateScale(qreal,qreal)));

    qDebug() << "sizeHint: " << _plot->sizeHint(); // 502px
    qDebug() << "sizeHint: " << _horizontalScrollView->sizeHint(); // 503px, tiene 1px más y la escrollbar también
    qDebug() << "sizeHint: " << _verticalScrollView->sizeHint(); // en resize cambia el ancho y se produce el efecto cubetera, es el layout
}
