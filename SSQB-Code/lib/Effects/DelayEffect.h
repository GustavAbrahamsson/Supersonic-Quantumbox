#include "GenericEffect.h"

#define MAX_DELAY 48000/4
#define NUM_CHANNELS 4

class DelayEffect : public GenericEffect
{
private:
    String name = "Delay";
    String InputNames[3] = {"RoomSize", "Spread", "Decay"};
    float InputValues[3] = {0.05, 0.1, 0.5};

    uint32_t delayTimes[4]; // number of samples to delay

    AudioBuffer<float> buffer = AudioBuffer<float>();

public:
    float DSP(float sample)
    {
        //delay and feedback
        float dry = sample * 0.5f;
        float wet = 0;

        for (uint32_t i = 0; i < NUM_CHANNELS; i++)
        {
            wet += buffer.read(delayTimes[i]) * (1.0f/NUM_CHANNELS); // read from feedback
        }
        wet = wet * InputValues[2]; //decay 
        
        buffer.write(dry + wet); // send to feedback

        return dry + wet;
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
            display->drawFastVLine(delayTimes[i] * (1/(MAX_DELAY/128.0f)), 30, 30, WHITE);
        }

        // Draw line for the current sample
        display->drawFastVLine(0, 30, 30, WHITE);

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

        if(index == 0 || index==1){
            uint32_t delay = (uint32_t)(MAX_DELAY*InputValues[0]);
            uint32_t spreadstep = delay * InputValues[1] * (1.0f/(NUM_CHANNELS-1));

            for(int i = 0; i < 4; i++){
                delayTimes[i] = delay - i*spreadstep;
            }
        }
    }
};