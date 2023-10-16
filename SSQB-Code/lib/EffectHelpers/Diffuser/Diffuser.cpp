#include "Diffuser.h"

void Diffuser::init(int MaxDelay, float alpha){
    this->alpha = alpha;
    this->samplesDelay = MaxDelay;
    buffer.init(MaxDelay);
}

void Diffuser::init(int MaxDelay, int samplesDelay, float alpha){
    this->alpha = alpha;
    this->samplesDelay = samplesDelay;
    buffer.init(MaxDelay);
}

float Diffuser::process(float sample){
    float delayedSample = buffer.read(samplesDelay);
    float output = sample * alpha + delayedSample;
    buffer.write(sample - alpha * delayedSample);
    return output;
}

void Diffuser::setAlpha(float alpha){
    this->alpha = alpha;
}

void Diffuser::setDelay(int samplesDelay){
    if(samplesDelay > MaxDelay) samplesDelay = MaxDelay;
    this->samplesDelay = samplesDelay;
}

