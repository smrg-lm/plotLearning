#include "visualwave.h"
#include "visualcontrolpoint.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include <QGraphicsView>
#include <QGraphicsScene>

#include <QDebug>


VisualWave::VisualWave(QGraphicsItem *parent)
    : VisualGroup(parent, QPointF(), QSizeF())
{}

VisualWave::VisualWave(QGraphicsItem *parent, const QPointF &pos)
    : VisualGroup(parent, pos, QSizeF())
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

    // no se puede llamar este update cada vez que actualiza
    // la vista por cualquier razón, no parece correcto, hay
    // que actualizar los path cuando sea necesario
    this->updatePathItems(lod); // confirmar que actualiza correctamente
}

void VisualWave::readSoundFile()
{
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(-1, 1);

    // create fake file data (disk data)
    int sr = 48000;
    _fileFrameSize = sr; // * 60 * 2; // 10'? ver QList index size
    for(unsigned long i = 0; i < _fileFrameSize; i++)
        fakeDiskAudioData.append(dist(e2));

    // create (fake) peak data (disk data)
    int peakWindowSize = 64;
    for(unsigned long i = 0; i < _fileFrameSize / peakWindowSize; i++) { // puede ignorar hasta las peakWindowSize-1 últimas muestras
        qreal peak = 0;
        for(int j = 0; j < peakWindowSize; j++) {
            if(std::fabs(fakeDiskAudioData[i+j]) > std::fabs(peak))
                peak = fakeDiskAudioData[i+j];
        }
        fakeDiskPeakData.append(peak); // parece que repite el mismo valor en ventanas consecutivas?
    }
    _peaksFrameSize = fakeDiskPeakData.size();

    // TEST
    bufferedData = fakeDiskPeakData;
    this->setBufferSize(_peaksFrameSize);
    this->setSampleRate(sr);
    this->setSize(QSizeF(_peaksFrameSize * this->graphicUnit(), 2));

    // TEST
    //bufferedData = fakeDiskAudioData;
    //this->setBufferSize(_fileFrameSize);
    //this->setSampleRate(sr);
    //this->setSize(QSizeF(_fileFrameSize * this->graphicUnit(), 2));
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
    qreal visualStartPos = this->floorQuant(vr.left(), _graphicUnit); // esto puede quedar afuera de lo visual, pero no es <0
    qreal visualEndPos = visualStartPos + this->floorQuant(vr.width(), _graphicUnit);
    unsigned long startPos = (unsigned long)(visualStartPos / _graphicUnit); // aunque no debería poder ser ulong
    unsigned long endPos = (unsigned long)(visualEndPos / _graphicUnit);

    qDebug() << "visual start: " << visualStartPos << "visual end: " << visualEndPos;
    qDebug() << "start pos: " << startPos << "end pos: " << endPos;

    QPainterPath wavePath; // esto debería actualizar solo si es necesario
    QPainterPath controlsPath;

    // waveShapeItem
    qreal x = visualStartPos;
    qreal y = this->linlin(bufferedData[startPos], -1, 1, 0, this->boundingRect().height());
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
    for(unsigned long i = startPos + 1; i < endPos; i++) {
        x = i * _graphicUnit;
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
    lastUpdateStartPos = startPos;
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
    bufferedData[lastUpdateStartPos + selectedPointNumber] = this->linlin(point.y(), 0, this->boundingRect().height(), -1, 1);
    this->update();
}
