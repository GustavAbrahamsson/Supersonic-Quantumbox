#ifndef SCOPE_H
#define SCOPE_H

#include "GenericEffect.h"

// Simple scope to view the signal
// 
class Scope: public GenericEffect{
private:
    
    float InputValues[1] = {0.1f};
    String InputNames[1] = {"Trigger"};


    // Idea is to write to one buffer and display the other.
    // When trigger is high, sample for 64 more samples and then switch which
    // buffer is being displayed and which is being written to.
    AudioBuffer<float> buffers[2];
    const uint32_t bufferSize = 128;
    uint32_t currentWriteBuffer = 0;

    // Trigger should be rising edge with hysterisis
    uint32_t sampleCount = 0;
    bool hysterisisLow = false;
    const float triggerHysteresis = 0.1f;

    // Zero display after x samples without trigger
    uint32_t zeroDisplayCounter = 0;
    const uint32_t zeroDisplayCounterMax = 10000;

    // Only look at every x samples 
    // samplerate = 48000 / samplesSkip, 
    const uint32_t samplesSkip = 4;
    uint32_t skipCounter = 0;

public:

    float DSP(float sample){
        // Only look at every x samples
        skipCounter++;
        if(skipCounter < samplesSkip){
            return sample;
        }
        skipCounter = 0;

        // Write to buffer
        buffers[currentWriteBuffer].write(sample);

        

        // Wait for trigger at middle of screen, otherwise fill buffer 
        if(sampleCount == 64){
            if (sample > InputValues[0] && hysterisisLow)
            {
                sampleCount++;
            }else if (sample < InputValues[0] - triggerHysteresis)
            {
                hysterisisLow = true;
            }
            zeroDisplayCounter++;
        }else{
            sampleCount++;
        }

        // Switch buffers
        if(sampleCount >= bufferSize){
            currentWriteBuffer = !currentWriteBuffer;
            sampleCount = 0;
            hysterisisLow = false;
            zeroDisplayCounter = 0;
        }

        return sample;
    }

    void Draw(Adafruit_SSD1306 * display){
        
        if (zeroDisplayCounter > zeroDisplayCounterMax)
        {
            display->drawFastHLine(0, 44, 128, WHITE);
            return;
        }

        // Draw current Display buffer
        for(uint32_t i = 0; i < bufferSize; i++){
            display->drawPixel(i, 44 - (buffers[!currentWriteBuffer].read(127-i) * 20), WHITE);
        }

    }

    void init(){
        buffers[0].init(bufferSize);
        buffers[1].init(bufferSize);
    }

    String getName(){
        return "Scope";
    }

    uint32_t getNumInputs(){
        return 1;
    }

    String getInputName(uint32_t index){
        if(index == 0)
            return InputNames[0];
        return "None";
    }

    float getInputValue(uint32_t index){
        if(index == 0)
            return InputValues[0];
        return 0.0f;
    }

    void setInputValue(uint32_t index, float value){
        if(index == 0)
            InputValues[0] = value;
    }

};

#endif