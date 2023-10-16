#ifndef LPF1_H
#define LPF1_H

// first-order LPF
class LPF1
{
private:
    float alpha;
    float old_out = 0;

public:
    LPF1(float alpha);

    float process(float in);

    float process(float in, float alpha);

    void setAlpha(float alpha);

    void reset();

    void reset(float value);
};

#endif