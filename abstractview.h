#ifndef BASEVIEW_H
#define BASEVIEW_H

#include <QGraphicsView>

// algún método debería ser pure abstract...
// por ahora es simplemente base class
class BaseView : public QGraphicsView
{
    Q_OBJECT

public:
    BaseView(QWidget *parent = 0);

    bool zoomOnResize() const { return _zoomOnResize; } // probar QGraphicsView::fitInView
    void setZoomOnResize(bool value) { _zoomOnResize = value; }
    void resizeEvent(QResizeEvent *event) override; // probar QGraphicsView::fitInView
    void scale(qreal sx, qreal sy) { // probar QGraphicsView::fitInView
        QGraphicsView::scale(sx, sy);
        emit scaleChanged(sx, sy);
    }

    // QGraphicsView transformationAnchor : ViewportAnchor
    // QGraphicsView::isInteractive()

    // para que los eventos lleguen a la escena hay que despacharlos a QGraphicsView
    // en caso de querer al implmentar estos responders
    void wheelEvent(QWheelEvent *event) override;

signals:
    void scaleChanged(qreal sx, qreal sy);

public slots:
    void updateScale(qreal sx, qreal sy) { QGraphicsView::scale(sx, sy); }
    void horizontalScrollByDelta(int d);
    void verticalScrollByDelta(int d);

protected:
    bool _zoomOnResize;
};

#endif // BASEVIEW_H
