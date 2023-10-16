#include "LPF1.h"

// first-order LPF
LPF1::LPF1(float alpha)
{
    this->alpha = alpha;
}

float LPF1::process(float in)
{
    // float out = alpha * in + (1 - alpha) * old_out;
    float out = old_out + alpha * (in - old_out); // save one multiplication
    old_out = out;
    return out;
}

float LPF1::process(float in, float alpha)
{
    this->alpha = alpha;
    float out = old_out + alpha * (in - old_out);
    old_out = out;
    return out;
}

void LPF1::setAlpha(float alpha)
{
    this->alpha = alpha;
}

void LPF1::reset()
{
    old_out = 0;
}

void LPF1::reset(float value)
{
    old_out = value;
}