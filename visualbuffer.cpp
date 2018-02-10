#include "visualbuffer.h"

#include <QDebug>

// it happens to be sc_mod
int negmod(int a, int b) {
    a = a % b;
    if(a < 0) return b + a;
    return a;
}

RangeRingBuffer::RangeRingBuffer(int size)
    : _size(size)
{
    for(int i = 0; i < _size; i++) _array.append(Peak());
    head = 0;
    tail = _size - 1;
}

void RangeRingBuffer::rightAdd(int offset, qreal value)
{
    // right add
    tail++;
    tail %= _size;
    _array[tail].offset = offset;
    _array[tail].value = value;
    // left pop
    head++;
    head %= _size;
}

void RangeRingBuffer::leftAdd(int offset, qreal value)
{
    // left add
    head--;
    head = negmod(head, _size);
    _array[head].offset = offset;
    _array[head].value = value;
    // right pop
    tail--;
    tail = negmod(tail, _size);
}

Peak RangeRingBuffer::getAt(int pos)
{
    return this->peak(pos);
}

void RangeRingBuffer::setAt(int pos, int offset, qreal value)
{
    this->peak(pos).offset = offset;
    this->peak(pos).value = value;
}

Peak & RangeRingBuffer::peak(int index)
{
    if(index < 0 || index >= _size)
        throw "RangeRingBuffer index out of range!";

    return _array[(head + index) % _size];
}



VisualBuffer::VisualBuffer(int size)
    : _size(size)
{
    _buffer = new RangeRingBuffer(_size);
}

VisualBuffer::~VisualBuffer()
{
    delete _buffer;
}

void VisualBuffer::readSoundFile()
{
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(-1, 1);

    // create fake file data (disk data)
    int sr = 48000;
    unsigned long fileFrameSize = sr * 60 * 2.5; // 10'? ver QList index size

    for(unsigned long i = 0; i < fileFrameSize; i++) {
        if(i % (sr/10) == 0) diskData.append(1); else diskData.append(0.);
        //diskData.append(dist(e2));
    }
    _frameSize = diskData.size();

    // create (fake) peak data (disk data)
    for(unsigned long i = 0; i < fileFrameSize / diskPeaksBlock; i++) { // puede ignorar hasta las peakWindowSize-1 últimas muestras
        qreal peak = 0;
        for(int j = 0; j < diskPeaksBlock; j++) {
            if(std::fabs(diskData[i*diskPeaksBlock+j]) > std::fabs(peak)) {
                peak = diskData[i*diskPeaksBlock+j];
            }
        }
        diskPeaks.append(peak);
    }
}

void VisualBuffer::update(unsigned long sp, unsigned long range)
{
    unsigned long auxReadSize;
    int auxLevel = 0;
    while(auxLevel < 32) {
        auxReadSize = _size * std::pow(2, auxLevel);
        if(range < auxReadSize) break;
        auxLevel += 1;
    }

    // no necesita actualizar, el buffer está cargado con lo necesario
    if(visualStart == sp
            && visualRange == range
            && visualLevel == auxLevel) return;

    previousStart = visualStart;
    visualStart = sp;

    previousRange = visualRange;
    visualRange = range;

    previousLevel = visualLevel;
    previousBlock = _visualBlock;
    visualLevel = auxLevel;
    _visualBlock = std::pow(2, visualLevel);

    if(_visualBlock != previousBlock) // llena lo necesario por única vez, cuando cambia el rango, qué pasa con el scroll?
        this->updateFill();
    else if(visualRange == previousRange) // supuestamente, si no cambió el nivel no cambió el rango, pero puede fallar por la resolución gráfica?
        this->updateScroll(); // OK, NO ANDA, ERA DE ESPERARSE... :-/
}

Peak VisualBuffer::calcPeak(unsigned long startOffset, unsigned long blockSize)
{
    int offset = 0;
    qreal peak = 0;
    qreal data;

    for(unsigned long i = 0; i < blockSize; i++) { // el rango de i es int

        data = diskData[startOffset + i]; // TEST

        if(std::fabs(data) > std::fabs(peak)) {
            offset = i;
            peak = data;
        }
    }

    return Peak(offset, peak);
}

void VisualBuffer::updateFill()
{
    // cambió el nivel
    qDebug() << "updateFill() _visualBlock = " << _visualBlock; // medir cpu para calcular caché de picos

    unsigned long blockOffset;
    Peak peak;

    for(int i = 0; i < _size; i++) {
        blockOffset = i * _visualBlock;

        if(visualStart + blockOffset + _visualBlock <= (unsigned long)diskData.size()) {

            peak = this->calcPeak(visualStart + blockOffset, _visualBlock);
            _buffer->setAt(i, peak.offset, peak.value);

        } else if(visualStart + blockOffset < (unsigned long)diskData.size()) {
            unsigned long remainderBlockSize = (unsigned long)diskData.size() - (visualStart + blockOffset);

            peak = this->calcPeak(visualStart + blockOffset, remainderBlockSize);
            _buffer->setAt(i, peak.offset, peak.value);

        } else {

            /*
             * no hay más data para llenar el buffer
             * el rango visual es más corto que el buffer
             */
            break;

        }
    }
}

void VisualBuffer::updateScroll()
{
    /*
     * - cambió de posición el rango pero no cambiaron ni el nivel
     *   ni el largo del rango
     * - left/right add "mueven" la posición del rango
     * - el buffer está completo pero puede no estar lleno si está
     *   al final del archivo, ver
     */

    qDebug() << "updateScroll() visualStart = " << visualStart
             << "previousStart = " << previousStart;

    unsigned long range;
    unsigned long blockOffset;
    Peak peak;

    if(previousStart > visualStart){
        range = previousStart - visualStart;

        qDebug() << "leftAdd, range = " << range;

        for(unsigned long i = 1; i <= range; i++) {
            blockOffset = i * _visualBlock;
            peak = this->calcPeak(previousStart - blockOffset, _visualBlock);
            _buffer->leftAdd(peak.offset, peak.value);
        }

    } else {
        range = visualStart - previousStart;
        unsigned long previousEnd = previousStart + previousRange;

        qDebug() << "rightAdd, range = " << range;

        for(unsigned long i = 0; i < range; i++) {
            if(previousEnd + i >= (unsigned long)diskData.size()) break; // no hay más para escrolear, comprobar
            blockOffset = i * _visualBlock;
            peak = this->calcPeak(previousEnd + blockOffset, _visualBlock);
            _buffer->rightAdd(peak.offset, peak.value);
        }

    }
}

void VisualBuffer::updateZoomOut()
{
    /*
     * - el nuevo rango contiene al rango previo
     * - pero puede exceder por ambos lados o solo por uno
     * - el nivel tiene que cambiar, en este caso es posible
     *   optimizar, aunque es más complicado, pero no para
     *   zoom in, y eso produce procesamiento asimétrico
     */
}

void VisualBuffer::updateZoomIn()
{
    /*
     * - el rango nuevo es un subconjunto del rango previo
     * - pero puede coincidir en el principio o el fin del rango anterior
     */
}
