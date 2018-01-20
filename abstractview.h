#ifndef ABSTRACTVIEW_H
#define ABSTRACTVIEW_H

#include <QGraphicsView>

// algún método debería ser pure abstract...
// por ahora es simplemente base class
class AbstractView : public QGraphicsView
{
    Q_OBJECT

public:
    AbstractView(QWidget *parent = 0);

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
    void updateScale(qreal sx, qreal sy) { QGraphicsView::scale(sx, sy); }
    void horizontalScrollByDelta(int d);
    void verticalScrollByDelta(int d);

protected:
    bool _zoomOnResize;
};

#endif // ABSTRACTVIEW_H
