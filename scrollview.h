#ifndef SCROLLVIEW_H
#define SCROLLVIEW_H

#include <QGraphicsView>

class ScrollView : public QGraphicsView
{
    Q_OBJECT

public:
    ScrollView(QWidget *parent = 0);
};

#endif // SCROLLVIEW_H
