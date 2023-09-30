#include "GenericEffect.h"

// Equivalent to infinite gain with a deadzoone
class Square : public GenericEffect{
    private:
        String name = "Squarify";
        float InputValues[1] = {0.2f};
        float gain = 0.3f;
        float deadzone = 0.003f;

    public:

        float DSP(float sample){
            // Pass through
            if (this->pass)
                return sample;
                
            if (sample > deadzone)
                return 1.0f*gain;
            else if(sample < -deadzone)
                return -1.0f*gain;
            else
                return 0.0f;
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
                    return "Volume";
                default:
                    return "Error";
            }
        }

        float getInputValue(uint32_t index){
            if(index > 0)
                return 0.0f;
            return InputValues[index];
        }

        void setInputValue(uint32_t index, float value){
            if(index > 0)
                return;
            InputValues[index] = value;
            gain = InputValues[0];
        }
};