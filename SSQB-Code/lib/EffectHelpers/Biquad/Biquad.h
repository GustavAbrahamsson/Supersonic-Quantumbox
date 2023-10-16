#ifndef BIQUAD_H
#define BIQUAD_H

class Biquad
{
private:
    float old_x[2] = {0, 0};
    float old_y[2] = {0, 0};

    // Normalized coefficients (a0 = 1)
    float num[3] = {0, 0, 0};
    float denom[2] = {0, 0};

public:
    Biquad();

    Biquad(float num0, float num1, float num2, float denom1, float denom2);

    // if 3 denom, normalize by denom[0]
    Biquad(float num0, float num1, float num2, float denom0, float denom1, float denom2);

    float process(float in);

    void setCoeffs(float num0, float num1, float num2, float denom1, float denom2);

    // if 3 denom, normalize by denom[0]
    void setCoeffs(float num0, float num1, float num2, float denom0, float denom1, float denom2);
};

#endif