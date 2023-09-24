#include "GenericEffect.h"

// Volume meter, displays max with decay.
class Meter : public GenericEffect
{
private:
    String name = "Level Meter";
    float InputValues[0];
    float max = 0;

public:
    float DSP(float sample)
    {
        // Max sample with decay
        if (sample > max)
        {
            max = sample;
        }else{
            max -= max*0.001f; // set decay rate
        }

        return sample;
    }

    void Draw(Adafruit_SSD1306 *display)
    {
        // bar VU meter
        int barHeight = (int) (max * 40.0f);
        float pct = max * 100.0f;
        
        display->fillRect(10, 60-barHeight, 50, barHeight, WHITE);
        display->drawRect(5, 20, 60, 40, WHITE);
        display->drawFastHLine(65, 20, 5, WHITE);
        display->drawFastHLine(65, 30, 3, WHITE);
        display->drawFastHLine(65, 40, 5, WHITE);
        display->drawFastHLine(65, 50, 3, WHITE);
        display->drawFastHLine(65, 59, 5, WHITE);

        display->setCursor(80, 30);
        display->print(pct);
        display->print("%");
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

    float getInputValue(uint32_t index)
    {
        return InputValues[index];
    }

    void setInputValue(uint32_t index, float value)
    {
        InputValues[index] = value;
    }
};