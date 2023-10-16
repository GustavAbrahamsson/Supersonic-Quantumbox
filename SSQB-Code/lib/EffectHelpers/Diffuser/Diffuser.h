#include "AudioBuffer/AudioBuffer.h"

// Schroeder all-pass filter
class Diffuser{
    private:

        AudioBuffer<float> buffer;
        float alpha = 0.5;
        int MaxDelay = 2000;
        int samplesDelay = 0;

    public:

        void init(int MaxDelay, float alpha);

        void init(int MaxDelay, int samplesDelay, float alpha);

        float process(float sample);

        void setAlpha(float alpha);

        void setDelay(int samplesDelay);

};
