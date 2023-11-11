#include "Parameter.h"

//default constructor
Parameter::Parameter(){
    this->name = "Parameter";
    this->min = 0.0f;
    this->max = 1.0f;
    this->scale = PARAM_LINEAR;

    range = max - min;
    rangeOver3 = range / 3.0f;
    rangeOver7 = range / 7.0f;
}

void Parameter::config(String name, float min, float max, ParameterScale scale)
{
    this->name = name;
    this->min = min;
    this->max = max;
    this->scale = scale;

    range = max - min;
    rangeOver3 = range / 3.0f;
    rangeOver7 = range / 7.0f;
}

float Parameter::get(){
    return value;
}

String Parameter::getName(){
    return name;
}

void Parameter::set(float rawValue){
    if(rawValue < 0)
        rawValue = 0;
    if(rawValue > 1)
        rawValue = 1;

    switch (scale){
        case(PARAM_LINEAR):
            value = rawValue * (max - min) + min;
            break;
        case(PARAM_LOG):
            value = rangeOver3 * log2f(7.0f*rawValue + 1.0f) + min;
            break;
        case(PARAM_EXP):
            value = rangeOver7 * (exp2f(3.0f*rawValue) - 1.0f) + min;
            break;
    }

}
