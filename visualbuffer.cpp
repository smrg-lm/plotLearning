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
        //if(i % (sr/10) == 0) diskData.append(1); else diskData.append(0.);
        diskData.append(dist(e2));
    }
    _frameSize = diskData.size();

    this->buildCache();
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

    if(_visualBlock != previousBlock)
        this->updateFill();
    else
        this->updateScroll();
}

Peak VisualBuffer::calcPeak(unsigned long startOffset, unsigned long blockSize)
{
    // los bloques en el disco son siempre los mismos a distintos blockSize
    unsigned long startBlockOffset = startOffset - startOffset % blockSize;

    if(startBlockOffset + blockSize <= (unsigned long)diskData.size()) {
        // ok, blockSize = blockSize...
    } else if(startBlockOffset < (unsigned long)diskData.size()) {
        blockSize = (unsigned long)diskData.size() - startBlockOffset; // remainder
    } else {
        return Peak(); // out of range
    }

    int offset = 0;
    qreal peak = 0;
    qreal data;

    for(unsigned long i = 0; i < blockSize; i++) { // el rango de i es int
        data = diskData[startBlockOffset + i]; // TEST
        if(std::fabs(data) > std::fabs(peak)) {
            offset = i;
            peak = data;
        }
    }

    return Peak(offset, peak);
}

// repite porque no se me ocurre cómo hacer fácilmente que la función calcPeak sea genérica
Peak VisualBuffer::calcCachePeak(unsigned long startOffset, unsigned long blockSize,
                                const QList<Peak> & cacheData)
{
    unsigned long startBlockOffset = startOffset - startOffset % blockSize;

    if(startBlockOffset + blockSize <= (unsigned long)cacheData.size()) {
        // ok, blockSize = blockSize...
    } else if(startBlockOffset < (unsigned long)cacheData.size()) {
        blockSize = (unsigned long)cacheData.size() - startBlockOffset; // remainder
    } else {
        throw "VisualBuffer: Error building cache, index out of range!";
    }

    int offset = 0;
    qreal peak = 0;
    Peak data;

    for(unsigned long i = 0; i < blockSize; i++) { // el rango de i es int
        data = cacheData[startBlockOffset + i]; // TEST
        if(std::fabs(data.value) > std::fabs(peak)) {
            offset = i * diskCacheBlock + data.offset; // confirmar
            peak = data.value;
        }
    }

    /*
     * desde afuera, offset en muestras es dentro del bloque anterior
     * según el nivel:
     * en diskCache[0][n] es n * diskCacheBlock + offset
     * en diskCache[1][n] es n * pow(diskCacheBlock, 2) + offset
     * en diskCache[2][n] es n * pow(diskCacheBlock, 3) + offset
     * ...
     * en diskCache[l][n] es n * pow(diskCacheBlock, l+1) + offset
     * offset es int
     * confirmar
     */

    return Peak(offset, peak);
}

void VisualBuffer::buildCache()
{
    diskCache.append(QList<Peak>());
    for(unsigned long i = 0; i < (unsigned long)diskData.size(); i += diskCacheBlock) {
        diskCache[0].append( this->calcPeak(i, diskCacheBlock) );
    }

    for(int l = 1; l <= 2; l++) {
        diskCache.append(QList<Peak>());
        for(unsigned long i = 0; i < (unsigned long)diskCache[l-1].size(); i += diskCacheBlock) {
            diskCache[l].append( this->calcCachePeak(i, diskCacheBlock, diskCache[l-1]) );
        }
    }

    qDebug() << "diskCache[0].size(): " << diskCache[0].size();
    qDebug() << "diskCache[1].size(): " << diskCache[1].size();
    qDebug() << "diskCache[2].size(): " << diskCache[2].size();

    // para 48000 / 10 == 4800
    qDebug() << "diskCache[0][300].offset: " << diskCache[0][300].offset;
    qDebug() << "diskCache[0][300].value: " << diskCache[0][300].value; // 4800 / 16 == 300;
    qDebug() << "diskCache[1][18].offset " << diskCache[1][18].offset;
    qDebug() << "diskCache[1][18].value " << diskCache[1][18].value; // 300 / 16 == 18.75
    qDebug() << "diskCache[2][1].offset: " << diskCache[2][1].offset;
    qDebug() << "diskCache[2][1].value: " << diskCache[2][1].value; // 18.75 / 16 == 1.171875

    // *** los offset pueden estar mal para la función VisualWave::updateSignalPath, ver
    // *** el offset tiene que ser int, es dentro del bloque a cada nivel

    /*
     * 1024   -> b1
     * 2048   -> 2
     * 4096   -> 4
     * 8192   -> 8
     * 16384  -> 16  -> b1(16)
     * 32768  -> 32  -> 2
     * 65536  -> 64  -> 4
     * 131072 -> 128 -> 8
     * 262144 -> 256 -> 16 -> b1(256)
     *
     * i.e. 48000 / 16 == 3000
     *      3000 / 16 == 187.5
     *      187.5 / 16 == 11.7
     *      11.7 * 60 * 60 == 42120
     *
     * i.e. 3000 * 8 * 60**2 / 1024**2 == 82.3974609375
     *      187.5 * 8 * 60**2 / 1024**2 == 5.14984130859375
     *      11.7 * 8 * 60**2 / 1024**2 == 0.32135009765625
     *
     * i.e. 87.86865234375M por una hora, se puede optimizar
     * bajando la resolución y subiendo diskCacheBlock, afectaría
     * solamente lo gráfico en zoom lejano, pero lo pensé como
     * pares int-float que es lo más fácil
     */
}

void VisualBuffer::updateFill()
{
    /*
     * cambia el nivel, itera sobre _size bloques
     */

    unsigned long blockOffset;
    Peak peak;

    for(int i = 0; i < _size; i++) {
        blockOffset = i * _visualBlock;
        peak = this->calcPeak(visualStart + blockOffset, _visualBlock);
        _buffer->setAt(i, peak.offset, peak.value);
    }
}

void VisualBuffer::updateScroll()
{
    /*
     * no cambia el nivel, _size mide bloques, fill llena el
     * buffer por bloques iterando sobre size, esta función
     * tiene que convertir visualStart a bloques o _size a
     * muestras, range es en bloques
     */

    unsigned long range;
    unsigned long blockOffset;
    Peak peak;

    if(previousStart > visualStart) {
        range = previousStart / _visualBlock - visualStart / _visualBlock;

        for(unsigned long i = 1; i <= range; i++) {
            blockOffset = i * _visualBlock;
            peak = this->calcPeak(previousStart - blockOffset, _visualBlock);
            _buffer->leftAdd(peak.offset, peak.value);
        }

    } else {
        range = visualStart / _visualBlock - previousStart / _visualBlock;
        unsigned long previousEnd = previousStart + _size * _visualBlock;

        for(unsigned long i = 0; i < range; i++) {
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
     * - no veo optimización posible para este caso
     */
}
