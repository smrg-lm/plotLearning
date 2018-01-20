#ifndef PLOTVIEW_H
#define PLOTVIEW_H

#include "abstractview.h"

class WorkArea;

class PlotView : public AbstractView
{
    Q_OBJECT

public:
    PlotView(QWidget *parent = 0);

    WorkArea *workArea() const;
    void setWorkArea(WorkArea *workArea);
    QRectF waRect() const;

protected:
    void mouseDoubleClickEvent(QMouseEvent *event); Q_DECL_OVERRIDE
    void mousePressEvent(QMouseEvent *event); Q_DECL_OVERRIDE
    void mouseMoveEvent(QMouseEvent *event); Q_DECL_OVERRIDE
    void mouseReleaseEvent(QMouseEvent *event); Q_DECL_OVERRIDE

private:
    QGraphicsScene *_scene;
    WorkArea *_workArea;
};

#endif // PLOTVIEW_H
