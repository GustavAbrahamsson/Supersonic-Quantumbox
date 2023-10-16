#include "MaxFilter.h"

MaxFilter::MaxFilter() {}

MaxFilter::MaxFilter(float decay)
{
    this->decay = decay;
}

float MaxFilter::process(float in)
{
    if (in > max)
        max = in;
    else
        max *= decay;
    return max;
}

void MaxFilter::setDecay(float decay)
{
    this->decay = decay;
}

void MaxFilter::reset()
{
    max = 0;
}