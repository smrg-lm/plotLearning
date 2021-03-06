#ifndef PLOTVIEW_H
#define PLOTVIEW_H

#include "baseview.h"

class WorkArea;

class PlotView : public BaseView
{
    Q_OBJECT

public:
    PlotView(QWidget *parent = 0);

    WorkArea *workArea() const;
    void setWorkArea(WorkArea *workArea);
    QRectF waRect() const;

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QGraphicsScene *_scene;
    WorkArea *_workArea;
};

#endif // PLOTVIEW_H
