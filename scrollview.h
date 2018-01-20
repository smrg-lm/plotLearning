#ifndef SCROLLVIEW_H
#define SCROLLVIEW_H

#include "abstractview.h"

class RuleArea;

class ScrollView : public AbstractView
{
    Q_OBJECT

public:
    ScrollView(QWidget *parent = 0);

    QSize sizeHint() const { return _sizeHint; } Q_DECL_OVERRIDE
    Qt::Orientation orientation() const { return _orientation; }
    void setOrientation(Qt::Orientation o);

    RuleArea *ruleArea() const;
    void setRuleArea(RuleArea *ruleArea);

private:
    QSize _sizeHint;
    Qt::Orientation _orientation;
    QGraphicsScene *_scene;
    RuleArea *_ruleArea;
};

#endif // SCROLLVIEW_H
