#ifndef VISUALBUFFER_H
#define VISUALBUFFER_H

#include <QList>

struct Peak
{
    Peak(int offset = 0, qreal value = 0)
        : offset(offset), value(value) {}
    Peak(const Peak &p)
        : offset(p.offset), value(p.value) {}
    int offset;
    qreal value;
};

// tengo que repasar el uso de referencias, valores y punteros
class RangeRingBuffer
{
public:
    RangeRingBuffer(int size);

    int size() { return _size; }
    void rightAdd(int offset, qreal value);
    void leftAdd(int offset, qreal value);
    Peak getAt(int pos);
    void setAt(int pos, int offset, qreal value);

private:
    Peak & peak(int index);

    QList<Peak> _array; // QList simplemente porque es más fácil por ahora
    int _size;
    int head;
    int tail;
};

class VisualBuffer
{
public:
    VisualBuffer(int size = 1024);
    ~VisualBuffer();

    // file access interface...
    void readSoundFile();
    int sampleRate() { return _sampleRate; }
    unsigned long frameSize() { return _frameSize; }

    // buffer access interface...
    const RangeRingBuffer &buffer() const { return *_buffer; }
    int size() { return _size; }
    unsigned long visualBlock() { return _visualBlock; }

    void update(unsigned long sp, unsigned long range);

private:
    Peak calcPeak(unsigned long startOffset, unsigned long blockSize);
    Peak calcCachePeak(unsigned long startOffset, unsigned long blockSize,
                      const QList<Peak> & cacheData);
    void buildCache();
    void updateFill();
    void updateScroll();

    // maybe not useful, not implemented
    void updateZoomOut();
    void updateZoomIn();

    // TEST
    QList<qreal> diskData;

    QList<QList<Peak>> diskCache;
    int diskCacheLevels = 3;
    int diskCacheBlock = 16;

    int _sampleRate = 48000;
    unsigned long _frameSize;

    RangeRingBuffer *_buffer;
    int _size;

    // estas inicializaciones no deberían estar, creo
    unsigned long visualStart = -1;
    unsigned long visualRange = 0;
    unsigned long _visualBlock = -1;
    int visualLevel = -1;

    unsigned long previousStart = -1;
    unsigned long previousRange = 0;
    unsigned long previousBlock = -1; // not used
    int previousLevel = -1; // not used
};

#endif // VISUALBUFFER_H
