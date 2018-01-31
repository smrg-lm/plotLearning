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
    //qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    // no se puede llamar este update cada vez que actualiza
    // la vista por cualquier razón, no parece correcto, hay
    // que actualizar los path cuando sea necesario
    this->updatePathItems(); //(lod); // confirmar que actualiza correctamente
}

void VisualWave::readSoundFile()
{
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(-1, 1);

    // create fake file data (disk data)
    int sr = 48000;
    _fileFrameSize = sr * 60 * 2; // 10'? ver QList index size
    for(unsigned long i = 0; i < _fileFrameSize; i++)
        fakeDiskAudioData.append(dist(e2));

    // create (fake) peak data (disk data)
    for(unsigned long i = 0; i < _fileFrameSize / peaksBlockSize; i++) { // puede ignorar hasta las peakWindowSize-1 últimas muestras
        qreal peak = 0;
        for(int j = 0; j < peaksBlockSize; j++) {
            if(std::fabs(fakeDiskAudioData[i*peaksBlockSize+j]) > std::fabs(peak)) {
                peak = fakeDiskAudioData[i*peaksBlockSize+j];
            }
        }
        fakeDiskPeakData.append(peak);
    }
    _peaksFrameSize = fakeDiskPeakData.size();

    // init buffer data (precario, hay que calcular _bufferFrameSize)
    for(int i = 0; i < _bufferFrameSize; i++)
        bufferedData.append(0.0);

    this->setSampleRate(sr);
    this->setSize(QSizeF(_fileFrameSize * this->graphicUnit(), 2));
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

void VisualWave::updatePathItems()
{
    // es mejor no mantener las vistas no activas así
    // el elemento se puede visualizar con distintas vistas
    // usando los mismos recursos

    QRectF vr = this->visibleRect();
    qreal visualStartPos = this->floorQuant(vr.left(), _graphicUnit); // esto puede quedar afuera de lo visual, pero no es <0
    qreal visualEndPos = visualStartPos + this->floorQuant(vr.width(), _graphicUnit);
    unsigned long startPos = (unsigned long)(visualStartPos / _graphicUnit); // sí, puede ser ulong
    unsigned long endPos = (unsigned long)(visualEndPos / _graphicUnit);

    this->updateSignalPath(startPos, endPos);
    // LOD es endPos - startPos
    //this->updateControlPointsPath(startPos, endPos);
    //controlPointsItem.setPath(QPainterPath()); // clear...

    lastUpdateStartPos = startPos;
}

void VisualWave::calcPeaks(unsigned long sp, unsigned long ep,
                           unsigned long range, const QList<qreal> &diskData)
{
    // range *must be* >= _bufferFrameSize
    unsigned long stepSize = range / _bufferFrameSize; // repite en llamadora
    unsigned long resto = range % _bufferFrameSize;
    qreal peak, data;

    //qDebug() << "Welcome to Calc Peaks";

    for(unsigned long i = 0; i < (unsigned long)_bufferFrameSize - 1; i++) { // el rango de i es int
        peak = 0;
        for(unsigned long j = 0; j < stepSize; j++) { // el rango de j es int
            data = diskData[sp + (stepSize * i) + j];
            if(std::fabs(data) > std::fabs(peak)) peak = data;
        }
        bufferedData[i] = peak;
    }

    //qDebug() << "Before remainder";

    // remainder
    peak = 0;
    for(unsigned long j = 0; j < resto; j++) {
        data = diskData[ep - 1 - j]; // backwards from ep
        if(data > peak) peak = data;
    }
    bufferedData[_bufferFrameSize - 1] = peak;
}

void VisualWave::updateSignalPath(unsigned long sp, unsigned long ep)
{
    if(sp == prevStartPos && ep == prevEndPos) return; // far sight
    prevStartPos = sp; prevEndPos = ep;

    /*
     * Importante:
     * - Tal vez haya que interpolar, pero checkear los rangos visuales
     * en relación al dibujo de los path para que no se generen saltos
     * en la densidad visual.
     */

    // data
    unsigned long range = ep - sp; // ep - sp es range, es LOD
    if(range <= (unsigned long)_bufferFrameSize) {
        // samples
        for(unsigned long i = 0; i < range; i++) { // el rango de i es int
            bufferedData[i] = fakeDiskAudioData[sp + i];
        }
    } else if(range <= (unsigned long)peaksBlockSize * _bufferFrameSize) {
        // picos de samples
        this->calcPeaks(sp, ep, range, fakeDiskAudioData);
    } else {
        /*
         * range/peaksBlockSize tiene que ser mayor que _bufferSize, entonces
         * el problema es cuándo (range / peaksBlockSize)/_bufferSize es igual o mayor que 1,
         * despejando: range == peaksBlockSize * _bufferSize, por lo tanto la condición
         * anterior tiene que ser range <= peaksBlockSize * _bufferSize para que
         * este else no crashee. Pero esa relación no garantiza nada visualmente... :)
         * Ver que la cantidad de operaciones para obtener los picos sea relativamente
         * acotada y constante (cpu, importante).
         */
        // picos de picos
        qDebug() << "PICOS DE PICOS";
        this->calcPeaks(sp/peaksBlockSize, ep/peaksBlockSize, range/peaksBlockSize, fakeDiskPeakData);
    }

    // graphics
    QPainterPath wavePath; // esto debería actualizar solo si es necesario
    qreal x = sp * _graphicUnit;
    qreal y = this->linlin(bufferedData[0], -1, 1, 0, this->boundingRect().height());

    unsigned long stepSize = range / _bufferFrameSize;
    // cierra por la condición del elif (peaksBlockSize * _bufferFrameSize), la sucesión es continua
    qDebug() << "*********** stepSize: " << stepSize;

    int loopSize = (int)std::min((unsigned long)_bufferFrameSize, range);
    qDebug() << "*********** Loop size: " << loopSize;

    wavePath.moveTo(x, y);
    for(int i = 1; i < loopSize; i++) {
        x = (sp + i * (stepSize + 1)) * _graphicUnit; // falta remainder?
        // dibuja fuera de pantalla hacia la derecha?
        y = this->linlin(bufferedData[i], -1, 1, 0, this->boundingRect().height());
        wavePath.lineTo(x, y);
    }

    qDebug() << "Assign path";
    waveShapeItem.setPath(wavePath);
}

void VisualWave::updateControlPointsPath(unsigned long sp, unsigned long ep)
{
    // *** pointers are uwrong
    QPainterPath controlsPath;
    QSizeF controlPointSize(this->calculateUntrasnformedFactorSize(QSize(controlPointRadio, controlPointRadio)));
    qreal x = sp * _graphicUnit;
    qreal y = this->linlin(bufferedData[sp], -1, 1, 0, this->boundingRect().height());

    controlsPath.addEllipse(QPointF(x, y),
                            controlPointSize.width(),
                            controlPointSize.height());
    for(unsigned long i = sp + 1; i < ep; i++) {
        x = i * _graphicUnit;
        y = this->linlin(bufferedData[i], -1, 1, 0, this->boundingRect().height());
        controlsPath.addEllipse(QPointF(x, y),
                                controlPointSize.width(),
                                controlPointSize.height());
    }

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
    bufferedData[lastUpdateStartPos + selectedPointNumber] = this->linlin(point.y(), 0, this->boundingRect().height(), -1, 1);
    this->update();
}
