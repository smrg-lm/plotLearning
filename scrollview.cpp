#include "scrollview.h"
#include "rulearea.h"

ScrollView::ScrollView(QWidget *parent)
    : AbstractView(parent)
{
    _sizeHint = QSize(25, 25);
    this->setMinimumSize(_sizeHint);
    this->setOrientation(Qt::Horizontal);

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _scene = new QGraphicsScene(this); // ownership
    this->setScene(_scene);

    // basic top level scene/visual element (QGraphicsView::sceneRect)
    this->setRuleArea(new RuleArea());
    //_scene->setSceneRect(this->waRect());
}

void ScrollView::setOrientation(Qt::Orientation o)
{
    _orientation = o;
    if(_orientation == Qt::Horizontal) {
        this->setSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Preferred);
    } else {
        this->setSizePolicy(
                    QSizePolicy::Preferred,
                    QSizePolicy::Ignored);
    }
}

RuleArea *ScrollView::ruleArea() const
{
    return _ruleArea;
}

void ScrollView::setRuleArea(RuleArea *ruleArea)
{
    // check nil, remove from scene, destroy? make private?
    _ruleArea = ruleArea;
    _scene->addItem(_ruleArea);
}
