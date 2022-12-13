// Audio buffer class
// Overwrites oldest data when buffer is full
// stores data in PSRAM

#include <Arduino.h>

template <typename T>
class AudioBuffer
{
private:
    T * buffer;
    uint32_t head;
    uint32_t tail;
    uint32_t size;

public:
    // constructor
    AudioBuffer(uint32_t size){
        buffer = (T *)ps_malloc(size * sizeof(T));
        this->size = size;
        head = 0;
        tail = 0;
    };

    // destructor
    ~AudioBuffer(){
        free(buffer);
    };

    // write data to buffer
    void write(T data){
        buffer[head] = data;
        head = (head + 1) % size;
    };

    // read data from buffer
    // index: number of samples back
    T read(uint32_t index){
        uint32_t i = (head - index) % size;
        return buffer[i];
    };
};