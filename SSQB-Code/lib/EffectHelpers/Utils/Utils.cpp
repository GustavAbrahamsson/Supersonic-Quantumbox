// Helper functions and classes for effects
#include "Utils.h"

// fast square root
// This is undefined code, but works on ESP32-s2 so it's (probably) fine
// Calculates 1/sqrt(f) using quake alg, then sqrt(f) = f* 1/sqrt(f)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

float f_sqrt(float f)
{
        long i;
        float x2, y;
        const float threehalfs = 1.5F;

        x2 = f * 0.5F;
        y = f;

        i = *(long *)&y;           // evil floating point bit level hacking
        i = 0x5f3759df - (i >> 1); // what the fuck?
        y = *(float *)&i;
        y = y * (threehalfs - (x2 * y * y)); // 1st iteration
        // y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

        return f * y;
}

#pragma GCC diagnostic pop