#ifndef VISUALWAVE_H
#define VISUALWAVE_H

#include "visualgroup.h"

class ControlPoint;

class VisualWave : public VisualGroup
{
    Q_OBJECT

public:
    VisualWave(QGraphicsItem *parent, const QPointF &pos, const QSizeF &size);
    using VisualGroup::VisualGroup;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

    qreal sampleRate() { return _sampleRate; }
    void setSampleRate(qreal value) {
        _sampleRate = value;
        _graphicUnit = 1. / _sampleRate;
    }
    qreal graphicUnit() { return _graphicUnit; }
    unsigned long bufferSize() { return _bufferSize; }

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    qreal linlin(qreal value, qreal inMin, qreal inMax, qreal outMin, qreal outMax);
    QSizeF calculateUntrasnformedFactorSize(const QSizeF &size) const;
    void updatePathItems(const qreal &lod);
    int obtainPointNumber(const QPointF &point);
    void editPoint(const QPointF &point);

    QList<qreal> fakeData;

    QGraphicsPathItem waveShapeItem;
    QGraphicsPathItem controlPointsItem;
    qreal controlPointLOD = 6; // is more hirizontal than vertical
    qreal controlPointRadio = 5;
    bool pointSelected = false;
    int selectedPointNumber = -1;

    int visualStartPos = 0;
    int visualEndPos = 0;

    qreal _sampleRate = 48000;
    qreal _graphicUnit = 1. / _sampleRate;
    unsigned long _bufferSize = 0;
};

#endif // VISUALWAVE_H

/*
 * Possible graphical horizontal unit:
 * Math.pow(2, -32) == 2.3283064365386963e-10
 * with qreal as double, but not sure,
 * at all (INT_MAX is a problem), cambiar
 * qslider puede ser mejor solución.
 * PERO: si la unidad es el segundo hay
 * 32767 / 60 ^ 2 ~ 9.1 horas independientemente
 * de la frecuencia de muestreo.
 *
 * El rango vertical se tiene que poder escalar
 * para igualar distintos rangos de datos, tal
 * vez, llegado el caso, se puede hacer lo mismo
 * con la escala temporal, pero no sería necesario
 * por ahora.
 *
 * Add edit mode:
 * By sample (with tooltip) vs drawing
 *
 * Plotter modes:
 * linear (default)
 * points (could be non editable edit points)
 * plines (points an lines)
 * levels (just the level line by sample not bars
 * steps
 *
 * Plotter also have resolution as visual sr
 * however it doesn't prevent visual aliasing
 *
 * Other visualizations useful for audio:
 * rms (pero puede que esto sea información "de" los datos)
 * lpf at sr (analog imitation)
 * bars (lin and bilin, sticks)
 *
 * Paths need to be optimezed for large data
 */
