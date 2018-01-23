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
