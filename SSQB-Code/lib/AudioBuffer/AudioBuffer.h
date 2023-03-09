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
    AudioBuffer(){
    };

    void init(uint32_t size){
        buffer = (T *)ps_malloc(size * sizeof(T));
        this->size = size;
        head = 0;
    }

    // destructor
    ~AudioBuffer(){
        free(buffer);
    };

    // write data to buffer
    void write(T data){
        head++;
        buffer[head] = data;
        if (head >= size){
            head = 0;
        }
    };

    // read data from buffer
    // index: number of samples back
    T read(uint32_t index){
        if (index >= size){
            return 0;
        }else if(index >= head){
            return(buffer[size + head - index]);
        }else{
            return(buffer[head - index]);
        } 
    };
};

#endif