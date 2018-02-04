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

    // no se puede llamar este update cada vez que actualiza
    // la vista por cualquier razón, no parece correcto, hay
    // que actualizar los path cuando sea necesario
    this->updatePathItems(); // confirmar que actualiza correctamente
}

void VisualWave::readSoundFile()
{
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(-1, 1);

    // create fake file data (disk data)
    int sr = 48000;
    _fileFrameSize = sr * 60 * 2; // 10'? ver QList index size
    for(unsigned long i = 0; i < _fileFrameSize; i++) {
        if(i % (sr/1) == 0) fakeDiskAudioData.append(1); else fakeDiskAudioData.append(0);
        // hay que comprobar que la unidad gráfica y los picos den información visual coherente
        //fakeDiskAudioData.append(dist(e2));
    }

    // create (fake) peak data (disk data)
    for(unsigned long i = 0; i < _fileFrameSize / peaksBlockSize; i++) { // puede ignorar hasta las peakWindowSize-1 últimas muestras
        qreal peak = 0;
        for(int j = 0; j < peaksBlockSize; j++) {
            if(std::fabs(fakeDiskAudioData[i*peaksBlockSize+j]) > std::fabs(peak)) {
                peak = fakeDiskAudioData[i*peaksBlockSize+j];
            }
        }
        fakeDiskPeaksData.append(peak);
    }
    _peaksFrameSize = fakeDiskPeaksData.size();

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

QSizeF VisualWave::cufs(const QSizeF &size) const
{
    // Calculate Untrasnformed Factor Size
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
    unsigned long startPos = (unsigned long)(visualStartPos / _graphicUnit);
    unsigned long endPos = (unsigned long)(visualEndPos / _graphicUnit);

    if(startPos == lastUpdateStartPos && endPos == lastUpdateEndPos) // e.g. far sight
        return;

    this->updateSignalPath(startPos, endPos);
    this->updateControlPointsPath(startPos, endPos, visualEndPos - visualStartPos);

    lastUpdateStartPos = startPos;
    lastUpdateEndPos = endPos;
}

void VisualWave::updateSignalPath(unsigned long sp, unsigned long ep)
{
    unsigned long range = ep - sp;

    // data
    this->updateBufferedData(sp, range);

    // graphics
    QPainterPath wavePath;
    qreal x = sp * _graphicUnit;
    qreal y = this->linlin(bufferedData[0], 1, -1, 0, this->boundingRect().height());
    wavePath.moveTo(x, y);

    int loopSize = range / currentReadBlockSize; // falta el remanente, da int dentro de _bufferFrameSize

    // no dibuja en scroll, siempre de 0 sp
    for(int i = 1; i < loopSize; i++) {
        x = sp + i * _graphicUnit * currentReadBlockSize;
        y = this->linlin(bufferedData[i], 1, -1, 0, this->boundingRect().height());
        wavePath.lineTo(x, y);
    }

    waveShapeItem.setPath(wavePath);
}

/*
 * Idea algoritmo:
 *
 * - el buffer carga el doble de lo que muestra 1 a 1 (1:1), para
 *   una sola imagen
 * - se va transformando gráficamente (se escala el path) hasta que
 *   pasa a ser 2:1 (en realidad < 2:1)
 * - se re-carga el buffer con los picos del doble (2:1)
 * - se va transformando....
 *
 * - para los picos se guarda (cachea) el valor del pico y el offset
 *   dentro del bloque
 * - los picos se recalculan cuando duplican en base al buffer
 *   anterior para la primera mitad (la totalidad se es ascendente)
 * - aún no estoy considerando el scroll
 *
 * Tal vez no funcione computacionalmente hablando, tal vez sea
 * crear una "imagen", para distintos niveles de zoom, pero luego
 * está el problema de la edición. El Audacity tiene que tener algún
 * algoritmo práctico. Igualmente lo más importante tienen que ser
 * los picos.
 */

void VisualWave::updateBufferedData(unsigned long sp, unsigned long range)
{
    qDebug() << "START OF UPDATE";

    unsigned long auxBufferSize;
    int auxPow2 = 0;
    while(auxPow2 < 32) {
        auxBufferSize = _bufferFrameSize * std::pow(2, auxPow2);
        if(range < auxBufferSize) break;
        auxPow2 += 1;
    }
    currentReadPowN = auxPow2;
    currentReadBlockSize = std::pow(2, currentReadPowN);
    QList<qreal> diskData = fakeDiskAudioData; // test

    qDebug() << "START POS: " << sp;
    qDebug() << "RANGE: " << range;
    qDebug() << "BLOCK LEVEL: " << currentReadPowN;
    qDebug() << "BLOCK SIZE: " << currentReadBlockSize;
    qDebug() << "READ SIZE: " << _bufferFrameSize * currentReadBlockSize;
    qDebug() << "DATA SIZE: " << diskData.size();

    for(int i = 0; i < _bufferFrameSize; i++) {
        unsigned long blockOffset = i * currentReadBlockSize;
        qreal peak, data;

        if(sp + blockOffset + currentReadBlockSize < (unsigned long)diskData.size()) {
            peak = 0;
            for(unsigned long j = 0; j < currentReadBlockSize; j++) { // el rango de j es int
                data = diskData[sp + blockOffset + j];
                if(std::fabs(data) > std::fabs(peak)) peak = data;
            }
            bufferedData[i] = peak;
        } else {
            // resto
        }
    }

    qDebug() << "END OF UPDATE";
}

/*
void VisualWave::calcPeaks(unsigned long sp, unsigned long ep,
                           unsigned long range, const QList<qreal> &diskData)
{
}
*/

//void VisualWave::updateBufferedData(unsigned long sp, unsigned long ep)
//{
//    /*
//     * Importante:
//     * - Tal vez haya que interpolar, pero checkear los rangos visuales
//     * en relación al dibujo de los path para que no se generen saltos
//     * en la densidad visual.
//     */

//    // data
//    unsigned long range = ep - sp; // ep - sp es range, es LOD
//    if(range < (unsigned long)_bufferFrameSize) {
//        // samples
//        for(unsigned long i = 0; i < range; i++) { // el rango de i es int
//            bufferedData[i] = fakeDiskAudioData[sp + i];
//        }
//    } else if(range < (unsigned long)peaksBlockSize * _bufferFrameSize) {
//        // picos de samples
//        this->calcPeaks(sp, ep, fakeDiskAudioData);
//    } else {
//        /*
//         * range/peaksBlockSize tiene que ser mayor que _bufferSize, entonces
//         * el problema es cuándo (range / peaksBlockSize)/_bufferSize es igual o mayor que 1,
//         * despejando: range == peaksBlockSize * _bufferSize, por lo tanto la condición
//         * anterior tiene que ser range <= peaksBlockSize * _bufferSize para que
//         * este else no crashee. Pero esa relación no garantiza nada visualmente... :)
//         * Ver que la cantidad de operaciones para obtener los picos sea relativamente
//         * acotada y constante (cpu, importante).
//         */
//        // picos de picos
//        this->calcPeaks(sp/peaksBlockSize, ep/peaksBlockSize, fakeDiskPeaksData);
//    }
//}

//void VisualWave::calcPeaks(unsigned long sp, unsigned long ep,
//                           const QList<qreal> &diskData)
//{
//    // range *must be* >= _bufferFrameSize
//    unsigned long range = ep - sp;
//    unsigned long stepSize = range / _bufferFrameSize; // repite en llamadora
//    unsigned long resto = range % _bufferFrameSize;
//    qreal peak, data;

//    for(unsigned long i = 0; i < (unsigned long)_bufferFrameSize - 1; i++) { // el rango de i es int
//        peak = 0;
//        for(unsigned long j = 0; j < stepSize; j++) { // el rango de j es int
//            data = diskData[sp + (stepSize * i) + j];
//            if(std::fabs(data) > std::fabs(peak)) peak = data;
//        }
//        bufferedData[i] = peak;
//    }

//    // remainder
//    peak = 0;
//    for(unsigned long j = 0; j < resto; j++) {
//        data = diskData[ep - 1 - j]; // backwards from ep
//        if(data > peak) peak = data;
//    }
//    bufferedData[_bufferFrameSize - 1] = peak;
//}

void VisualWave::updateControlPointsPath(unsigned long sp, unsigned long ep, qreal visualRange)
{
    if(!visualRange) return;

    QPainterPath controlsPath;
    QSizeF controlPointSize(this->cufs(QSize(controlPointRadio, controlPointRadio)));

    if((ep - sp) * controlPointSize.width() * 3 > visualRange) { // 3 -> 0.5 space...
        controlPointsItem.setPath(controlsPath); // clear, always...
        return;
    }

    qreal x = sp * _graphicUnit;
    qreal y = this->linlin(bufferedData[sp], 1, -1, 0, this->boundingRect().height());

    controlsPath.addEllipse(QPointF(x, y),
                            controlPointSize.width(),
                            controlPointSize.height());
    for(unsigned long i = sp + 1; i < ep; i++) {
        x = i * _graphicUnit;
        y = this->linlin(bufferedData[i], 1, -1, 0, this->boundingRect().height());
        controlsPath.addEllipse(QPointF(x, y),
                                controlPointSize.width(),
                                controlPointSize.height());
    }

    controlPointsItem.setPath(controlsPath);
}

int VisualWave::obtainPointNumber(const QPointF &point)
{
    // las posiciones coinciden con las unidades del gráfico
    QList<QPolygonF> points = controlPointsItem.path().toSubpathPolygons();

    for(int i = 0; i < points.size(); i++) {
        if(points[i].containsPoint(point, Qt::OddEvenFill)) return i;
    }

    // may fail depending on the polygon representation
    return -1;
}

void VisualWave::editPoint(const QPointF &point)
{
    /*
     * REDO: edited data can't be in buffered visual data
     * this is not cpu friendly (yet)
     * needs some kind of range and resolution control
     * enable tooltip showing values in rt
     */
    if(point.y() > this->boundingRect().height() || point.y() < 0) return;
    qreal newValue = this->linlin(point.y(), 0, this->boundingRect().height(), 1, -1);
    if(newValue > 1) newValue = 1; if(newValue < -1) newValue = -1;
    //bufferedData[lastUpdateStartPos + selectedPointNumber] = this->linlin(point.y(), 0, this->boundingRect().height(), 1, -1);
    this->update();
}
