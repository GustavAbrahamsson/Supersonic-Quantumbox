#ifndef MAXFILTER_H
#define MAXFILTER_H

class MaxFilter
{
private:
    float max = 0;
    float decay = 0.99f;

public:
    MaxFilter();

    MaxFilter(float decay);

    float process(float in);

    void setDecay(float decay);

    void reset();
};

#endif