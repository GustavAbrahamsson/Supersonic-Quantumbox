#include "Biquad.h"

Biquad::Biquad() {}

Biquad::Biquad(float num0, float num1, float num2, float denom1, float denom2)
{
    this->setCoeffs(num0, num1, num2, denom1, denom2);
}

// if 3 denom, normalize by denom[0]
Biquad::Biquad(float num0, float num1, float num2, float denom0, float denom1, float denom2)
{
    this->setCoeffs(num0, num1, num2, denom0, denom1, denom2);
}

float Biquad::process(float in)
{
    // Shift x and y buffers
    float x2 = old_x[1];
    old_x[1] = old_x[0];
    old_x[0] = in;

    float y2 = old_y[1];
    old_y[1] = old_y[0];

    // Biquad direct form
    old_y[0] = num[0] * in + num[1] * old_x[1] + num[2] * x2 - denom[1] * old_y[1] - denom[2] * y2;

    return old_y[0];
}

void Biquad::setCoeffs(float num0, float num1, float num2, float denom1, float denom2)
{
    this->num[0] = num0;
    this->num[1] = num1;
    this->num[2] = num2;
    this->denom[0] = denom1;
    this->denom[1] = denom2;
}

// if 3 denom, normalize by denom[0]
void Biquad::setCoeffs(float num0, float num1, float num2, float denom0, float denom1, float denom2)
{
    float scale = 1.0f / denom0;
    this->num[0] = num0 * scale;
    this->num[1] = num1 * scale;
    this->num[2] = num2 * scale;
    this->denom[0] = denom1 * scale;
    this->denom[1] = denom2 * scale;
}