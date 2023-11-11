#ifndef GenericEffect_h
#define GenericEffect_h

#include "EffectHelpers.h"
#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"


class GenericEffect{
    private:
        Parameter params[6];
        uint32_t numParams = 0;

        virtual void updateParam(uint32_t index){};

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

        uint32_t getNumParams(){
            return numParams;
        };

        String getParamName(uint32_t index){
            if (index >= numParams){ return "None"; }
            return params[index].getName();
        };

        float getParamValue(uint32_t index){
            if (index >= numParams){ return 0.0f; }
            return params[index].get();
        };

        void setParamValue(uint32_t index, float value){
            if (index >= numParams){ return; }
            params[index].set(value);
            updateParam(index);
        };

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