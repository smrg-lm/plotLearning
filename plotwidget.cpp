#include "plotwidget.h"
#include "plotview.h"
#include "scrollview.h"
#include "rulearea.h"
#include "workarea.h"

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

    _plot = new PlotView();
    _layout->addWidget(_plot, 1, 0);

    // sync horizontal scroll
    _horizontalScrollView = new ScrollView();
    _horizontalScrollView->ruleArea()->setSize(QSizeF(200, 25)); // depende de wa
    _layout->addWidget(_horizontalScrollView, 0, 0);

    connect(_horizontalScrollView->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            _plot->horizontalScrollBar(), SLOT(setValue(int)));
    connect(_plot->horizontalScrollBar(), SIGNAL(valueChanged(int)),
             _horizontalScrollView->horizontalScrollBar(), SLOT(setValue(int)));

    // sync vertical scroll
    _verticalScrollView = new ScrollView();
    _verticalScrollView->setOrientation(Qt::Vertical);
    _verticalScrollView->ruleArea()->setSize(QSizeF(25, 200)); // depende de wa
    _layout->addWidget(_verticalScrollView, 1, 1);

    connect(_verticalScrollView->verticalScrollBar(), SIGNAL(valueChanged(int)),
            _plot->verticalScrollBar(), SLOT(setValue(int)));
    connect(_plot->verticalScrollBar(), SIGNAL(valueChanged(int)),
            _verticalScrollView->verticalScrollBar(), SLOT(setValue(int)));

    _plot->setZoomOnResize(false); // TEST
    // sync zoom
    connect(_plot, SIGNAL(scaleChanged(qreal,qreal)),
            _horizontalScrollView, SLOT(updateScale(qreal,qreal)));
    connect(_horizontalScrollView, SIGNAL(scaleChanged(qreal,qreal)),
            _plot, SLOT(updateScale(qreal,qreal)));

    connect(_plot, SIGNAL(scaleChanged(qreal,qreal)),
            _verticalScrollView, SLOT(updateScale(qreal,qreal)));
    connect(_verticalScrollView, SIGNAL(scaleChanged(qreal,qreal)),
            _plot, SLOT(updateScale(qreal,qreal)));

    _plot->scale(20000, 300);
    //_horizontalScrollView->scale(10000, 200);
    //_verticalScrollView->scale(10000, 200);
}
