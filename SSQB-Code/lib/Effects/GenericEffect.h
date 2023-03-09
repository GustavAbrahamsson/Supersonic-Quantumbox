#include "AudioBuffer.h"
#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#ifndef GenericEffect_h
#define GenericEffect_h

class GenericEffect{
    public:
        // DSP function
        virtual int32_t DSP(int32_t sample) = 0;
        
        // Draw function
        virtual void Draw(Adafruit_SSD1306 * display) = 0;

        // Name of effect
        virtual String getName() = 0;

        // number of parameters
        virtual uint32_t getNumInputs() = 0;

        // Name of parameters
        virtual String getInputName(uint32_t index) = 0;

        // get parameter values
        virtual uint32_t getInputValue(uint32_t index) = 0;

        // set parameter values
        virtual void setInputValue(uint32_t index, uint32_t value) = 0;

};

#endif