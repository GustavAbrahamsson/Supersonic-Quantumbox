#include "GenericEffect.h"

#define MAX_DELAY 48000
#define NUM_CHANNELS 3

class DelayEffect : public GenericEffect
{
private:
    String name = "Delay";
    String InputNames[3] = {"RoomSize", "Decay"};
    float InputValues[3] = {0.5f, 0.2f};

    float oldFB = 0;

    uint32_t delayTimes[NUM_CHANNELS]; // delay times for each channel
    float delaySteps[NUM_CHANNELS] = {0.42f, 0.63f, 1.0f}; // delay steps for each channel

    AudioBuffer<float> buffer = AudioBuffer<float>();

public:
    float DSP(float sample)
    {
        // delay and feedback
        // Percussive elements are more pronounced
        float dry = sample * 0.5f;
        float wet = 0.0f;

        for (uint32_t i = 0; i < NUM_CHANNELS; i++)
        {
            wet += buffer.read(delayTimes[i]) * (1.0f/NUM_CHANNELS); // read from feedback
        }
        wet = wet * InputValues[1]; //decay 
        
        float FB = dry + wet;

        buffer.write(FB); // send to feedback

        return FB;

        // Consider https://ccrma.stanford.edu/~dattorro/EffectDesignPart1.pdf for reverb design
    }

    void init()
    {
        buffer.init(MAX_DELAY);   
    }

    void Draw(Adafruit_SSD1306 *display)
    {
        // Draw lines for each channel corresponding to the delay time
        for (uint32_t i = 0; i < NUM_CHANNELS; i++)
        {
            display->drawFastVLine(delayTimes[i] * (1/(MAX_DELAY/128.0f)), 32 + 32*(1.0f-InputValues[1]), 32, WHITE);
        }

        // Draw line for the current sample
        display->drawFastVLine(0, 32, 32, WHITE);

    }

    String getName()
    {
        return name;
    }

    uint32_t getNumInputs()
    {
        return 2;
    }

    String getInputName(uint32_t index)
    {
        return InputNames[index];
    }

    float getInputValue(uint32_t index)
    {
        return InputValues[index];
    }

    void setInputValue(uint32_t index, float value)
    {
        InputValues[index] = value;

        if(index == 0){
            uint32_t delay = (uint32_t)(MAX_DELAY*InputValues[0]);

            for(int i = 0; i < NUM_CHANNELS; i++){
                delayTimes[i] = delay * delaySteps[i];
            }
        }
    }
};