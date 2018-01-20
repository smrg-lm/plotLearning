#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>

class QGridLayout;
class PlotView;
class ScrollView;

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    PlotWidget(QWidget *parent = 0);

private:
    QGridLayout *_layout;
    PlotView *_plot;
    ScrollView *_horizontalScrollView;
    ScrollView *_verticalScrollView;
};

#endif // PLOTWIDGET_H
