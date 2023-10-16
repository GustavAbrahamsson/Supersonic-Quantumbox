// Audio buffer class
// Overwrites oldest data when buffer is full
// stores data in PSRAM

#ifndef AudioBuffer_h
#define AudioBuffer_h

#include <Arduino.h>

template <typename T>
class AudioBuffer
{
private:
    T * buffer;
    uint32_t head = 0;
    uint32_t size = 0;

public:
    // constructor
    AudioBuffer();

    void init(uint32_t size);

    // destructor
    ~AudioBuffer();

    // write data to buffer
    void write(T data);

    // read data from buffer
    // index: number of samples back
    T read(uint32_t index);

    // write data to buffer and return oldest data
    T process(T data);
};

#endif