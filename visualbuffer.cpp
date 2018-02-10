#include "visualbuffer.h"

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

    // range change
    if(sp != visualStart) {
        previousStart = visualStart;
        visualStart = sp;
    }
    if(range != visualRange) {
        previousRange = visualRange;
        visualRange = range;
    }

    // level change
    if(auxLevel != visualLevel) {
        previousLevel = visualLevel;
        previousBlock = visualBlock;
        visualLevel = auxLevel;
        visualBlock = std::pow(2, visualLevel);
    }

    // shared range (para scroll) // CHEQUEAR SI CAMBIÓ EL RANGO PERO NO EL NIVEL, y ANTES DE LLAMAR
    unsigned long visualEnd = visualStart + visualRange;
    unsigned long previousEnd = previousStart + previousRange;
    sharedStart = -1; //sharedRange = 0;
    if(previousStart >= visualStart && previousStart < visualEnd) {
        // el nuevo rango se movió hacia la hizquierda o saltó
        if(previousEnd <= visualEnd) {
            // segmento incluido en su totalidad
            sharedStart = previousStart;
            sharedRange = previousRange;
        } else {
            // segmento incluido hacia el final
            sharedStart = previousStart;
            sharedRange = visualEnd - previousStart;
        }
    } else {
        // el nuevo rango se movió hacia la derecha o saltó
        if(previousEnd > visualStart && previousEnd <= visualEnd) {
            // segmento incluido hacia el principio
            sharedStart = visualStart;
            sharedRange = previousEnd - visualStart;
        }
    }

    /*
     * Sería algo tal vez importante que el scroll sea siempre
     * por múltiplos del bloque actual? Pero esto dependería
     * de la vista
     */
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

    unsigned long blockOffset;
    Peak peak;

    for(int i = 0; i < _size; i++) {
        blockOffset = i * visualBlock;

        if(visualStart + blockOffset + visualBlock <= (unsigned long)diskData.size()) {

            peak = this->calcPeak(visualStart + blockOffset, visualBlock);
            _buffer->setAt(i, peak.offset, peak.value);

        } else if(visualStart + blockOffset < (unsigned long)diskData.size()) {
            unsigned long remainderBlockSize = (unsigned long)diskData.size() - (visualStart + blockOffset);

            peak = this->calcPeak(visualStart + blockOffset, remainderBlockSize);
            _buffer->setAt(i, peak.offset, peak.value);

        } else {

            // no hay más data para llenar el buffer
            break;

        }
    }
}

void VisualBuffer::updateScroll()
{
    // cambió de posición el rango pero no cambiaron ni el nivel ni el largo del rango
    // left/right add "mueven" la posición del rango
    // el buffer está completo pero puede no estar lleno si está al final del archivo, ver

    unsigned long range;
    unsigned long blockOffset;
    Peak peak;

    if(previousStart > visualStart){
        range = previousStart - visualStart;

        for(unsigned long i = 1; i <= range; i++) {
            blockOffset = i * visualBlock;
            peak = this->calcPeak(previousStart - blockOffset, visualBlock);
            _buffer->leftAdd(peak.offset, peak.value);
        }

    } else {
        range = visualStart - previousStart;
        unsigned long previousEnd = previousStart + previousRange;

        for(unsigned long i = 0; i < range; i++) {
            if(previousEnd + i >= diskData.size()) break; // no hay más para escrolear, comprobar
            blockOffset = i * visualBlock;
            peak = this->calcPeak(previousEnd + blockOffset, visualBlock);
            _buffer->rightAdd(peak.offset, peak.value);
        }

    }
}

void VisualBuffer::updateZoomOut()
{
    // el nuevo rango contiene al rango previo
    // pero puede exceder por ambos lados o solo por uno
}

void VisualBuffer::updateZoomIn()
{
    // el rango nuevo es un subconjunto del rango previo
    // pero puede coincidir en el principio o el fin del rango anterior
}
