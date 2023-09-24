#include "GenericEffect.h"

// Simple gain
class Clip : public GenericEffect{
    private:
        #define MAXGAIN 10.0f
        String name = "Gain";
        float InputValues[1] = {1.0f/MAXGAIN};
        float gain = 1.0f;

    public:

        float DSP(float sample){
            // Gain 
            return sample * gain;
        }

        void Draw(Adafruit_SSD1306 * display){
            display->setCursor(0,30);
            display->print("Gain: ");
            display->println(gain);
        }

        String getName(){
            return name;
        }

        uint32_t getNumInputs(){
            return 1;
        }

        String getInputName(uint32_t index){
            switch(index){
                case 0:
                    return "Gain";
                default:
                    return "Error";
            }
        }

        float getInputValue(uint32_t index){
            return InputValues[index];
        }

        void setInputValue(uint32_t index, float value){
            InputValues[index] = value;
            gain = InputValues[0]*MAXGAIN;
        }
};