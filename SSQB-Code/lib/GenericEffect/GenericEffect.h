#include "AudioBuffer.h"
#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"


class GenericEffect{
    public:
        // DSP function
        virtual uint32_t DSP(AudioBuffer<uint32_t> * input, AudioBuffer<uint32_t> * output) = 0;
        
        virtual void Draw(Adafruit_SSD1306 * display) = 0;

        // name of effect
        String name;

        // number of parameters
        const uint32_t numInputs;

        // Name of parameters
        const String InputNames[10];

        // Parameter values
        uint32_t InputValues[];
};
