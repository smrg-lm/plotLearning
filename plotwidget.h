#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>

class QGridLayout;
class PlotView;
class ScrollArea;

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    PlotWidget(QWidget *parent = 0);

private:
    QGridLayout *_layout;
    PlotView *_plot;
    ScrollArea *_horizontalScrollArea; int _ix;
    ScrollArea *_verticalScrollArea; int _iy;
};

#endif // PLOTWIDGET_H
