#include "plotwidget.h"
#include "plotview.h"
#include "scrollview.h"
#include "rulearea.h"

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

    _horizontalScrollView = new ScrollView();
    _horizontalScrollView->ruleArea()->setRect(QRectF(0, 0, 500, 25));
    _layout->addWidget(_horizontalScrollView, 0, 0);
    //connect scroll

    _verticalScrollView = new ScrollView();
    _verticalScrollView->setOrientation(Qt::Vertical);
    _verticalScrollView->ruleArea()->setRect(QRectF(0, 0, 25, 500));
    _layout->addWidget(_verticalScrollView, 1, 1);
    //connect scroll

    _plot->setZoomOnResize(true); // TEST
    //_horizontalScrollView->setZoomOnResize(true); // TEST
    connect(_plot, SIGNAL(scaleChanged(qreal,qreal)),
            _horizontalScrollView, SLOT(updateScale(qreal,qreal))); // los valores son distintos
    //_verticalScrollView->setZoomOnResize(true); // TEST
    connect(_plot, SIGNAL(scaleChanged(qreal,qreal)),
            _verticalScrollView, SLOT(updateScale(qreal,qreal))); // idem distinto
}
