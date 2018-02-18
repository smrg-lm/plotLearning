#include "visualwave.h"
#include "visualcontrolpoint.h"
#include "visualbuffer.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include <QGraphicsView>
#include <QGraphicsScene>

#include <QDebug>

// sc linlin
qreal linlin(qreal value, qreal inMin, qreal inMax, qreal outMin, qreal outMax)
{
    // sc linlin: (this-inMin)/(inMax-inMin) * (outMax-outMin) + outMin;
    return (value - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
}

VisualWave::VisualWave(QGraphicsItem *parent)
    : VisualGroup(parent, QPointF(), QSizeF())
{}

VisualWave::VisualWave(QGraphicsItem *parent, const QPointF &pos)
    : VisualGroup(parent, pos, QSizeF())
{
    // QGraphicsPathItem (QPainterPath)
    QPen pen; pen.setWidth(0);
    controlPointsItem.setParentItem(this);
    controlPointsItem.setPen(pen);
    controlPointsItem.setBrush(Qt::gray);
    signalItem.setParentItem(this);
    signalItem.setPen(pen);
    //signalItem.setBrush(Qt::black);

    // TEST
    // inicializa visual buffer y setea los parámetros gráficos
    visualBuffer = new VisualBuffer();
    visualBuffer->readSoundFile();
    this->setSampleRate(visualBuffer->sampleRate());
    this->setSize(QSizeF(visualBuffer->frameSize() * this->graphicUnit(), 50)); // 50 en vez de dos distorsiona los puntos de control cufs calculados
}

VisualWave::~VisualWave()
{
    delete visualBuffer;
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
    p.setBrush(Qt::black); // tal vez pasar p a updatePathItems?
    this->updatePathItems(); // confirmar que actualiza correctamente
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
    //if(vr.isEmpty()) return; // mantiene el último path pero perjudica el uso de más de una vista

    qreal visualStartPos = this->floorQuant(vr.left(), _graphicUnit); // esto puede quedar afuera de lo visual, pero no es <0
    //if(visualStartPos * _graphicUnit < vr.left()) visualStartPos += _graphicUnit; // pone dentro de lo visual, pero ver si es necesario, y la relación siguiente

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

/*
 * Idea algoritmo:
 *
 * - el buffer carga el doble de lo que muestra 1 a 1 (1:1), para
 *   una sola imagen, en realidad no es el doble, pero es más de
 *   lo que muestra, el cálculo de los picos es cada 2**n
 * - se va transformando gráficamente (se escala el path) hasta que
 *   pasa a ser 2:1
 * - se re-carga el buffer con los picos del doble (2:1)
 * - se va transformando....
 *
 * - para los picos se guarda (cachea) el valor del pico y el offset
 *   dentro del bloque
 * - los picos se recalculan cuando duplican en base al buffer
 *   anterior para la primera mitad (la totalidad se es ascendente)
 *
 * - si se puede guardar el buffer inmediato inferior para cuando el
 *   zoom va en alejamiento se pueden reutilizar valores pico dentro
 *   del nuevo rango. Esto no parece ser útil, es muy complicado y
 *   solo se puede hacer en dirección ascendente.
 *
 * - si el buffer es más grande por ambos lados se pueden reutilizar
 *   valores al hacer scroll
 *
 * - hay que calcular cómo dibuja al cambiar de escala para que no
 *   se generen saltos en la visualización
 * - permitir solo cambios de zoom en proporicones predeterminadas
 *   puede ser útil para la vizualización (cambio de escala) y el
 *   dibujo de los paths, también predefinir hacia qué lado se
 *   actualiza la información, ej. aparece desdes de derecha
 *
 * - la caché habría que calcularla en base al segundo como unidad
 *   y/o el tamaño del buffer como unidad de división, pero no sé
 *   exáctamente cómo por ahora
 * - pueden haber varios niveles de caché de tal manera que la cantidad

 *   de cálculos necesarios sea consntante dentro de cierto margen, la
 *   caché puede estar en memoria
 */

void VisualWave::updateSignalPath(unsigned long sp, unsigned long ep)
{
    unsigned long range = ep - sp;

    // data
    visualBuffer->update(sp, range);
    // graphics
    //this->linearPath(sp, range);
    this->stepsPath(sp, range);
    //this->levelsPath(sp, range);
    //this->barsPath(sp, range);
}

void VisualWave::linearPath(unsigned long sp, unsigned long range)
{
    // falta evitar el aliasing cuando se transforma el path
    // falta evitar el salto de densidad al cambiar el nivel

    RangeRingBuffer buffer = visualBuffer->buffer();
    QPainterPath signalPath;

    qreal x = (sp + buffer.getAt(0).offset) * _graphicUnit;
    qreal y = linlin(buffer.getAt(0).value, 1, -1, 0, this->boundingRect().height());
    signalPath.moveTo(x, y);

    int loopSize = range / visualBuffer->visualBlock(); // da int dentro de _bufferFrameSize

    //qDebug() << "buffer: " << buffer;
    //qDebug() << "loopSize: " << loopSize;

    for(int i = 1; i < loopSize; i++) {
        x = (sp + i * visualBuffer->visualBlock() + buffer.getAt(i).offset) * _graphicUnit; // esto queda bien con el cambio en calcPeak y la idea calcPeakPeak?
        y = linlin(buffer.getAt(i).value, 1, -1, 0, this->boundingRect().height());
        signalPath.lineTo(x, y);
    }

    signalItem.setPath(signalPath);
}

void VisualWave::stepsPath(unsigned long sp, unsigned long range)
{
    // la representación lejana del ruido blanco no es buena
    // tiene que ver con el aliasing en la tranasformación del path

    RangeRingBuffer buffer = visualBuffer->buffer();
    QPainterPath signalPath;

    qreal x = (sp + buffer.getAt(0).offset) * _graphicUnit;
    qreal y = linlin(buffer.getAt(0).value, 1, -1, 0, this->boundingRect().height());
    signalPath.moveTo(x, y);

    int loopSize = range / visualBuffer->visualBlock();

    for(int i = 1; i < loopSize; i++) {
        x = (sp + i * visualBuffer->visualBlock() + buffer.getAt(i).offset) * _graphicUnit; // esto queda bien con el cambio en calcPeak y la idea calcPeakPeak?
        signalPath.lineTo(x, y); // horizontal (dur), una instrución menos, la pendiente está sobre el cambio, estos path no dibujan la cola
        y = linlin(buffer.getAt(i).value, 1, -1, 0, this->boundingRect().height());
        signalPath.lineTo(x, y);
    }

    signalItem.setPath(signalPath);
}

void VisualWave::levelsPath(unsigned long sp, unsigned long range)
{
    // que no dibuje la cola es un problema

    RangeRingBuffer buffer = visualBuffer->buffer();
    QPainterPath signalPath;

    qreal x = (sp + buffer.getAt(0).offset) * _graphicUnit;
    qreal y = linlin(buffer.getAt(0).value, 1, -1, 0, this->boundingRect().height());
    signalPath.moveTo(x, y);

    int loopSize = range / visualBuffer->visualBlock();

    for(int i = 1; i < loopSize; i++) {
        x = (sp + i * visualBuffer->visualBlock() + buffer.getAt(i).offset) * _graphicUnit; // esto queda bien con el cambio en calcPeak y la idea calcPeakPeak?
        signalPath.lineTo(x, y); // horizontal (dur), una instrución menos, la pendiente está sobre el cambio, estos path no dibujan la cola
        y = linlin(buffer.getAt(i).value, 1, -1, 0, this->boundingRect().height());
        signalPath.moveTo(x, y); // única diferencia con stepsPath
    }

    signalItem.setPath(signalPath);
}

void VisualWave::barsPath(unsigned long sp, unsigned long range)
{
    // es muy pesado computacionalmente así como está
    // demasiado, por alguna razón, más de 200 rectángulos
    // y consume demasiado cpu al hacer zoom, raro?
    // el problema es signalItem.setBrush(Qt::black);

    // de cerca se ve lindo, pero usar un paths tampoco
    // serviría para ploteo espectral, toFillPolygons
    // podría ayudar, aunque no sirve para graphicsPathItem,
    // tal vez haya que hacerlo con graphicsRectItems?

    RangeRingBuffer buffer = visualBuffer->buffer();
    QPainterPath signalPath;
    int debugRectCount = 0;

    qreal x1 = (sp + buffer.getAt(0).offset) * _graphicUnit;
    qreal x2 = 0; // just because the compiler complains
    qreal y = linlin(buffer.getAt(0).value, 1, -1, 0, this->boundingRect().height());
    qreal yZero = linlin(0, 1, -1, 0, this->boundingRect().height());

    auto addRect = [&signalPath, &x1, &x2, &y, &yZero, &debugRectCount]() {
        if(y < yZero) {
            signalPath.addRect(x1, y, x2 - x1, -(y - yZero)); // up
            debugRectCount++;
        } else {
            signalPath.addRect(x1, y, x2 - x1, yZero - y); // down
            debugRectCount++;
        }
    };

    int loopSize = range / visualBuffer->visualBlock();

    for(int i = 1; i < loopSize; i++) {
        x2 = (sp + i * visualBuffer->visualBlock() + buffer.getAt(i).offset) * _graphicUnit;
        addRect();
        x1 = x2;
        y = linlin(buffer.getAt(i).value, 1, -1, 0, this->boundingRect().height());
    }
    addRect();

    qDebug() << "barsPath elementCount: " << signalPath.elementCount() << "rectCount: " << debugRectCount;
    signalItem.setPath(signalPath);
}

void VisualWave::updateControlPointsPath(unsigned long sp, unsigned long ep, qreal visualRange)
{
    if(!visualRange) return;

    QPainterPath controlsPath;
    QSizeF controlPointSize(this->cufs(QSize(controlPointRadio, controlPointRadio)));

    if((ep - sp) * controlPointSize.width() * 3 > visualRange) { // 3 -> 0.5 space...
        controlPointsItem.setPath(controlsPath); // clear, always...
        return;
    }

    RangeRingBuffer buffer = visualBuffer->buffer();
    qreal x = (sp + buffer.getAt(sp).offset) * _graphicUnit;
    qreal y = linlin(buffer.getAt(sp).value, 1, -1, 0, this->boundingRect().height());

    controlsPath.addEllipse(QPointF(x, y),
                            controlPointSize.width(),
                            controlPointSize.height());
    for(unsigned long i = sp + 1; i < ep; i++) {
        x = (i + buffer.getAt(i).offset) * _graphicUnit;
        y = linlin(buffer.getAt(i).value, 1, -1, 0, this->boundingRect().height());
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
    qreal newValue = linlin(point.y(), 0, this->boundingRect().height(), 1, -1);
    if(newValue > 1) newValue = 1; if(newValue < -1) newValue = -1;
    //bufferedData[lastUpdateStartPos + selectedPointNumber] = linlin(point.y(), 0, this->boundingRect().height(), 1, -1);
    this->update();
}
