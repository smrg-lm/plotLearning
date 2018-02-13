#ifndef VISUALWAVE_H
#define VISUALWAVE_H

#include "visualgroup.h"

class ControlPoint;
class VisualBuffer;

// VisualSignal?
class VisualWave : public VisualGroup
{
    Q_OBJECT

public:
    // size no es virtual, boundingRect es la variable virtual
    // por ahora size depende de _fileFrameSize para width
    // y 2 (rango bipolar) para height y se escala/transforma.
    VisualWave(QGraphicsItem *parent);
    VisualWave(QGraphicsItem *parent, const QPointF &pos);
    ~VisualWave();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

    void readSoundFile(); // All in one similar to Buffer.read, interface is not defined yet

    qreal sampleRate() { return _sampleRate; }
    void setSampleRate(qreal value) {
        _sampleRate = value;
        _graphicUnit = 1. / _sampleRate;
    }
    qreal graphicUnit() { return _graphicUnit; }

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QSizeF cufs(const QSizeF &size) const;

    void updatePathItems();
    void updateSignalPath(unsigned long sp, unsigned long ep);
    void linearPath(unsigned long sp, unsigned long range);
    void stepsPath(unsigned long sp, unsigned long range);
    void barsPath(unsigned long sp, unsigned long range);
    void updateControlPointsPath(unsigned long sp, unsigned long ep, qreal visualRange);

    int obtainPointNumber(const QPointF &point);
    void editPoint(const QPointF &point);

    QGraphicsPathItem signalItem;
    //QGraphicsPathItem wavePeaksItem;
    QGraphicsPathItem controlPointsItem;

    qreal controlPointRadio = 5;
    bool pointSelected = false;
    int selectedPointNumber = -1;

    unsigned long lastUpdateStartPos = 1;
    unsigned long lastUpdateEndPos = 0; // invalid pos, will be false (seek another way)

    VisualBuffer *visualBuffer;
    qreal _sampleRate = 48000;
    qreal _graphicUnit = 1. / _sampleRate; // can't be zero, by now
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
 * de la frecuencia de muestreo. Igualmente,
 * QGraphicsScene/View no tiene tanta resolución
 * hacia abajo.
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
 * linear (default) ok (test)
 * steps ok (test)
 * bars not ok (test)
 * points (could be non editable edit points)
 * plines (points an lines)
 * levels (just the level line by sample, not bars
 *
 * Múltiples señales en un mismo cajón se usa para
 * la visualización del tracking de armónicos
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
