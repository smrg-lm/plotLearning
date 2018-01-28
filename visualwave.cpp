#include "visualwave.h"
#include "visualcontrolpoint.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include <QGraphicsView>
#include <QGraphicsScene>

#include <QDebug>


VisualWave::VisualWave(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size)
    : VisualGroup(parent, pos, size)
{
    this->readSoundFile(); // TEST

    // QGraphicsPathItem (QPainterPath)
    QPen pen; pen.setWidth(0);
    controlPointsItem.setParentItem(this);
    controlPointsItem.setPen(pen);
    controlPointsItem.setBrush(Qt::gray);
    waveShapeItem.setParentItem(this);
    waveShapeItem.setPen(pen);
}

void VisualWave::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(Qt::lightGray);
    QPen p;
    p.setWidth(0);
    painter->setPen(p);
    painter->drawRect(this->boundingRect());

    // pero lod no indica diferencia h/v
    // tal ve hay que usar QGraphicsItem::deviceTransform (ver doc)
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    this->updatePathItems(lod); // confirmar que actualiza correctamente
}

void VisualWave::readSoundFile()
{
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(-1, 1);
    int sr = 48000;
    _fileFrameSize = sr * 60 * 2; // 10'? ver QList index size (it would also need peak data
    for(unsigned long i = 0; i < _fileFrameSize; i++)
        fakeDiskAudioData.append(dist(e2));

    //bufferedPeaksData;
    //peaksFrameSize

    // TEST
    bufferedData = fakeDiskAudioData;
    this->setBufferSize(_fileFrameSize);

    this->setSampleRate(sr);

    // HAY QUE SETEAR EL TAMAÑO DEL WIDGET PARA QUE NO SE ROMPA
    // PERO ESO ES LO QUE VA A CAMBIAR AHORA LUEGO.
    // QList<qreal>??(void) readFromDisk(); // params, fills in memory buffer
}

void VisualWave::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    VisualGroup::mouseDoubleClickEvent(event);
}

void VisualWave::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(controlPointsItem.contains(event->pos())) {
        selectedPointNumber = this->obtainPointNumber(event->pos());
        qDebug() << "selectePointNumber: " << selectedPointNumber;
        if(selectedPointNumber >= 0) pointSelected = true;
    } else {
        VisualGroup::mousePressEvent(event); // se necesita para itemmove en los tres handlers
    }
}

void VisualWave::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(pointSelected) {
        this->editPoint(event->pos());
    } else {
        VisualGroup::mouseMoveEvent(event);
    }
}

void VisualWave::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(pointSelected) {
        selectedPointNumber = -1;
        pointSelected = false;
    } else {
        VisualGroup::mouseReleaseEvent(event);
    }
}

qreal VisualWave::linlin(qreal value, qreal inMin, qreal inMax, qreal outMin, qreal outMax)
{
    // sc linlin: (this-inMin)/(inMax-inMin) * (outMax-outMin) + outMin;
    return (value - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
}

QSizeF VisualWave::calculateUntrasnformedFactorSize(const QSizeF &size) const
{
    // counter transform por point size without translantion
    // (QTransform::map documentation)
    QGraphicsView *view = this->getCurrentActiveView();
    qreal psX, psY;

    if(view) {
        QTransform t;
        qreal ox, oy;

        t = this->deviceTransform(view->viewportTransform()).inverted();
        ox = size.width();
        oy = size.height();

        psX = t.m11() * ox + t.m21() * oy;
        psY = t.m22() * oy + t.m12() * ox;
        if(!t.isAffine()) {
            qreal wp = t.m13() * ox + t.m23() * oy + t.m33();
            psX /= wp;
            psY /= wp;
        }

        return QSizeF(psX, psY);
    } else {
        return QSizeF();
    }
}

void VisualWave::updatePathItems(const qreal &lod)
{
    // se necesitaría mantener la última para cuando la vista
    // no está activa para que se pueda ver cuando la ventana
    // no está en foco, o ver si se puede cambiar la implementación.
    QRectF vr = this->visibleRect();
    visualStartPos = (int)std::floor(vr.left());
    visualEndPos = visualStartPos + (int)std::floor(vr.width());
    qDebug() << "visual start: " << visualStartPos << "visual end: " << visualEndPos;

    QPainterPath wavePath;
    QPainterPath controlsPath;

    // waveShapeItem
    qreal x = visualStartPos;
    qreal y = this->linlin(bufferedData[visualStartPos], -1, 1, 0, this->boundingRect().height());
    wavePath.moveTo(x, y);

    // controlPointsItem
    QSizeF controlPointSize(
                this->calculateUntrasnformedFactorSize(QSize(controlPointRadio, controlPointRadio)));
    if(lod > controlPointLOD) {
        controlsPath.addEllipse(QPointF(x, y),
                                controlPointSize.width(),
                                controlPointSize.height());
    }

    // both
    for(int i = visualStartPos + 1; i < visualEndPos; i++) {
        x = i;
        y = this->linlin(bufferedData[i], -1, 1, 0, this->boundingRect().height());
        wavePath.lineTo(x, y);
        if(lod > controlPointLOD) {
            controlsPath.addEllipse(QPointF(x, y),
                                    controlPointSize.width(),
                                    controlPointSize.height());
        }
    }

    waveShapeItem.setPath(wavePath);
    controlPointsItem.setPath(controlsPath);
}

int VisualWave::obtainPointNumber(const QPointF &point)
{
    // las posiciones coinciden con las unidades del gráfico como números enteros
    QList<QPolygonF> points = controlPointsItem.path().toSubpathPolygons();

    for(int i = 0; i < points.size(); i++) {
        if(points[i].containsPoint(point, Qt::OddEvenFill)) return i;
    }

    // may fail depending on the polygon representation
    return -1;
}

void VisualWave::editPoint(const QPointF &point)
{
    // this is not cpu friendly (yet)
    if(point.y() > this->boundingRect().height() || point.y() < 0) return;
    qreal newValue = this->linlin(point.y(), 0, this->boundingRect().height(), -1, 1);
    if(newValue > 1) newValue = 1; if(newValue < -1) newValue = -1;
    // this needs some kind of range and resolution control
    // and enable tooltip showing values in rt
    bufferedData[visualStartPos + selectedPointNumber] = this->linlin(point.y(), 0, this->boundingRect().height(), -1, 1);
    this->update();
}
