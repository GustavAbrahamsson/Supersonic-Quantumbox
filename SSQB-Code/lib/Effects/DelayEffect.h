#include "GenericEffect.h"

#define MAX_DELAY 48000/4
#define NUM_CHANNELS 4

class DelayEffect : public GenericEffect
{
private:
    String name = "Delay";
    String InputNames[3] = {"RoomSize", "Spread", "Decay"};
    uint32_t InputValues[3] = {100, 1000, 4000};

    uint32_t delayTimes[4]; // number of samples to delay

    AudioBuffer<int32_t> buffer = AudioBuffer<int32_t>();

public:
    int32_t DSP(int32_t sample)
    {
        //delay and feedback
        int32_t dry = sample/2;
        int32_t wet = 0;

        for (uint32_t i = 0; i < NUM_CHANNELS; i++)
        {
            wet += buffer.read(delayTimes[i])/NUM_CHANNELS;
        }
        
        buffer.write(dry + wet);

        return dry + wet/2;
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
            display->drawFastVLine(delayTimes[i] / (MAX_DELAY / 128.0), 30, 30, WHITE);
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

    uint32_t getInputValue(uint32_t index)
    {
        return InputValues[index];
    }

    void setInputValue(uint32_t index, uint32_t value)
    {
        InputValues[index] = value;

        if(index == 0 || index==1){
            uint32_t delay = InputValues[0]* (MAX_DELAY/8192.0);
            uint32_t spreadstep = delay * InputValues[1]/(8192.0 * (NUM_CHANNELS-1));

            for(int i = 0; i < 4; i++){
                delayTimes[i] = delay - i*spreadstep;
            }
        }
    }
};