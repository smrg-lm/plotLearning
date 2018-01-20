#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QWidget>

// ScrollArea choca con QScrollArea, este área es una scrollbar no una qt área
class ScrollArea : public QWidget
{
    Q_OBJECT

public:
    ScrollArea(QWidget *parent = 0);

    QSize sizeHint() const { return _sizeHint; } Q_DECL_OVERRIDE
    void paintEvent(QPaintEvent *event); Q_DECL_OVERRIDE
    Qt::Orientation orientation() const { return _orientation; }
    void setOrientation(Qt::Orientation o);

    int value() const { return -_value; }
public slots:
    void setValue(int value) { _value = -value; }
public:
    int range() const { return _range; }
    void setRange(int value) { _range = value; }

public slots:
    // acompaña a PlotView::sceneRect TEST
    void scale(qreal sx, qreal sy) { _scaleX *= sx; _scaleY *= sy; } // problema int representation

public:
    void mouseDoubleClickEvent(QMouseEvent *event); Q_DECL_OVERRIDE
    void mousePressEvent(QMouseEvent *event); Q_DECL_OVERRIDE
    void mouseMoveEvent(QMouseEvent *event); Q_DECL_OVERRIDE
    void mouseReleaseEvent(QMouseEvent *event); Q_DECL_OVERRIDE

signals:
    void deltaScrollChanged(int value);

private:
    QSize _sizeHint;
    Qt::Orientation _orientation;
    qreal _scaleX, _scaleY;
    int _value, _range; // slider properties
    int _ipos;
};

#endif // SCROLLAREA_H
