#include "AudioBuffer.h"

template <typename T>
AudioBuffer<T>::AudioBuffer(){}

template <typename T>
void AudioBuffer<T>::init(uint32_t size)
{
    buffer = (T *)ps_malloc(size * sizeof(T));
    this->size = size;
    head = 0;

    // clear buffer
    for (uint32_t i = 0; i < size; i++)
    {
        buffer[i] = 0;
    }
}

// destructor
template <typename T>
AudioBuffer<T>::~AudioBuffer()
{
    free(buffer);
}

// write data to buffer
template <typename T>
void AudioBuffer<T>::write(T data)
{
    head++;
    buffer[head] = data;
    if (head >= size)
    {
        head = 0;
    }
}

// read data from buffer
// index: number of samples back
template <typename T>
T AudioBuffer<T>::read(uint32_t index)
{
    if (index >= size)
    {
        return 0;
    }
    else if (index >= head)
    {
        return (buffer[size + head - index]);
    }
    else
    {
        return (buffer[head - index]);
    }
}

template <typename T>
T AudioBuffer<T>::process(T data)
{
    write(data);
    return read(size - 1);
}

template class AudioBuffer<float>;