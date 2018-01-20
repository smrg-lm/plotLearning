#ifndef PLOTVIEW_H
#define PLOTVIEW_H

#include <QGraphicsView>

class WorkArea;
class ScrollArea;

class PlotView : public QGraphicsView
{
    Q_OBJECT

public:
    PlotView(QWidget *parent = 0);

    WorkArea *workArea() const;
    void setWorkArea(WorkArea *workArea);
    QRectF waRect() const;

    // cuando excede el tamaño de sceneRect
    bool zoomOnResize() const { return _zoomOnResize; }
    void setZoomOnResize(bool value) { _zoomOnResize = value; }
    void resizeEvent(QResizeEvent *event); Q_DECL_OVERRIDE
    void scale(qreal sx, qreal sy) {
        QGraphicsView::scale(sx, sy);
        emit scaleChanged(sx, sy);
    }

signals:
    void scaleChanged(qreal sx, qreal sy);

public slots:
    void horizontalScrollByDelta(int d);
    void verticalScrollByDelta(int d);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event); Q_DECL_OVERRIDE
    void mousePressEvent(QMouseEvent *event); Q_DECL_OVERRIDE
    void mouseMoveEvent(QMouseEvent *event); Q_DECL_OVERRIDE
    void mouseReleaseEvent(QMouseEvent *event); Q_DECL_OVERRIDE

private:
    QGraphicsScene *_scene;
    WorkArea *_workArea;
    bool _zoomOnResize;
};

#endif // PLOTVIEW_H
