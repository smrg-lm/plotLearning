#include "plotwidget.h"
#include "plotview.h"
#include "scrollarea.h"

#include <QGridLayout>
#include <QScrollBar>

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget(parent)
{
    _layout = new QGridLayout(this);
    _layout->setSpacing(0);
    _layout->setMargin(0);

    _plot = new PlotView(this);
    _layout->addWidget(_plot, 1, 0);

    _horizontalScrollArea = new ScrollArea();
    _horizontalScrollArea->setRange((int)_plot->waRect().width());
    connect(_horizontalScrollArea, SIGNAL(deltaScrollChanged(int)),
            _plot, SLOT(horizontalScrollByDelta(int)));
    connect(_plot->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            _horizontalScrollArea, SLOT(setValue(int)));
    connect(_plot, SIGNAL(scaleChanged(qreal,qreal)),
            _horizontalScrollArea, SLOT(scale(qreal,qreal))); // el problema es que solo tiene que cambiar scaleX?
    //connect range changed
    _layout->addWidget(_horizontalScrollArea, 0, 0);

    _verticalScrollArea = new ScrollArea();
    _verticalScrollArea->setOrientation(Qt::Vertical);
    _verticalScrollArea->setRange((int)_plot->waRect().height());
    connect(_verticalScrollArea, SIGNAL(deltaScrollChanged(int)),
            _plot, SLOT(verticalScrollByDelta(int)));
    connect(_plot->verticalScrollBar(), SIGNAL(valueChanged(int)),
            _verticalScrollArea, SLOT(setValue(int)));
    connect(_plot, SIGNAL(scaleChanged(qreal,qreal)),
            _verticalScrollArea, SLOT(scale(qreal,qreal))); // el problema es que solo tiene que cambiar scaleY?
    //connect range changed
    _layout->addWidget(_verticalScrollArea, 1, 1);

    _plot->setZoomOnResize(true); // TEST
}
