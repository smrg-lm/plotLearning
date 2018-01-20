#ifndef SCROLLVIEW_H
#define SCROLLVIEW_H

#include "abstractview.h"

class RuleArea;

class ScrollView : public AbstractView
{
    Q_OBJECT

public:
    ScrollView(QWidget *parent = 0);

    //QSize minimumSizeHint();

    Qt::Orientation orientation() const { return _orientation; }
    void setOrientation(Qt::Orientation o);

    RuleArea *ruleArea() const;
    void setRuleArea(RuleArea *ruleArea);

    QSize minimumSizeHint() const { return QSize(25, 25); }

public slots:
    void updateScale(qreal sx, qreal sy);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event); Q_DECL_OVERRIDE
    void mousePressEvent(QMouseEvent *event); Q_DECL_OVERRIDE
    void mouseMoveEvent(QMouseEvent *event); Q_DECL_OVERRIDE
    void mouseReleaseEvent(QMouseEvent *event); Q_DECL_OVERRIDE
    void wheelEvent(QWheelEvent *event); Q_DECL_OVERRIDE

private:
    Qt::Orientation _orientation;
    QGraphicsScene *_scene;
    RuleArea *_ruleArea;
};

#endif // SCROLLVIEW_H
