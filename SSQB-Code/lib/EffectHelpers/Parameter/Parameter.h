#ifndef PARAMETER_H
#define PARAMETER_H

#include "Arduino.h"

enum ParameterScale{
    PARAM_LINEAR,
    PARAM_LOG,
    PARAM_EXP
};

class Parameter {
private:
    ParameterScale scale = PARAM_LINEAR;
    float min = 0.0f;
    float max = 1.0f;
    float value = 0.0f;
    String name = "Parameter";

    // Helper vars for calculation
    float range = 1.0f;
    float rangeOver3 = 0.33333333333333f; 
    float rangeOver7 = 0.14285714285714f;

public:
    Parameter();

    void config(String name, float min, float max, ParameterScale scale = PARAM_LINEAR);

    void set(float RawValue);

    String getName();

    float get();

};

#endif