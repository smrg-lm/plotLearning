#ifndef SCROLLVIEW_H
#define SCROLLVIEW_H

#include "baseview.h"

class RuleArea;

class ScrollView : public BaseView
{
    Q_OBJECT

public:
    ScrollView(QWidget *parent = 0);

    Qt::Orientation orientation() const { return _orientation; }
    void setOrientation(Qt::Orientation o);

    RuleArea *ruleArea() const;
    void setRuleArea(RuleArea *ruleArea);

    QSize minimumSizeHint() const { return QSize(25, 25); }

public slots:
    void updateScale(qreal sx, qreal sy);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    Qt::Orientation _orientation;
    QGraphicsScene *_scene;
    RuleArea *_ruleArea;
};

#endif // SCROLLVIEW_H
