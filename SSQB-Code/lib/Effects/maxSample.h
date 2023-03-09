#include "GenericEffect.h"

class MaxSample : public GenericEffect
{
private:
    String name = "Level Meter";
    uint32_t InputValues[0];
    int32_t max = 0;

public:
    int32_t DSP(int32_t sample)
    {
        // Max sample with decay
        if (sample > max)
        {
            max = sample;
        }else{
            max -= max/(1<<15); // set decay rate
        }

        return sample;
    }

    void Draw(Adafruit_SSD1306 *display)
    {
        // bar VU meter
        int barHeight = max / (INT32_MAX / 40);
        int pct = max / (INT32_MAX / 100);
        
        display->fillRect(10, 60-barHeight, 50, barHeight, WHITE);
        display->drawRect(5, 20, 60, 40, WHITE);
        display->drawFastHLine(65, 20, 5, WHITE);
        display->drawFastHLine(65, 30, 3, WHITE);
        display->drawFastHLine(65, 40, 5, WHITE);
        display->drawFastHLine(65, 50, 3, WHITE);
        display->drawFastHLine(65, 59, 5, WHITE);

        display->setCursor(80, 30);
        display->print(pct);
        display->print(" %");
    }

    String getName()
    {
        return name;
    }

    uint32_t getNumInputs()
    {
        return 0;
    }

    String getInputName(uint32_t index)
    {
        switch (index)
        {
        default:
            return "None";
        }
    }

    uint32_t getInputValue(uint32_t index)
    {
        return InputValues[index];
    }

    void setInputValue(uint32_t index, uint32_t value)
    {
        InputValues[index] = value;
    }
};