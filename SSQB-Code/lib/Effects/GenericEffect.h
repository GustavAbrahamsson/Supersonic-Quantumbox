#ifndef GenericEffect_h
#define GenericEffect_h

#include "EffectHelpers.h"
#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

class GenericEffect{
    public:
        bool pass = false;

        // DSP function
        virtual float DSP(float sample) = 0;
        
        // Draw function
        virtual void Draw(Adafruit_SSD1306 * display) = 0;

        // Name of effect
        virtual String getName() = 0;

        // number of parameters
        virtual uint32_t getNumInputs() = 0;

        // Name of parameters
        virtual String getInputName(uint32_t index) = 0;

        // get parameter values
        virtual float getInputValue(uint32_t index) = 0;

        // set parameter values
        virtual void setInputValue(uint32_t index, float) = 0;

        // Passthough toggle
        void passTroughToggle(){
            pass = !pass;
        };

        // get passthrough state
        bool getPassTroughState(){
            return pass;
        };

        // init function, called at startup (optional)
        virtual void init(){};

};

#endif