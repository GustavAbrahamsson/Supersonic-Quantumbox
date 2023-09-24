#include "GenericEffect.h"

class Clip : public GenericEffect{
    private:
        String name = "Clip";
        float InputValues[1] = {0.5};

    public:

        float DSP(float sample){
            // Clip me baby!
            float clip = InputValues[0]; 
        
            if(sample > clip){
                return clip;
            }
            if(sample < -clip){
                return -clip;
            }
        
            return sample;
        }

        void Draw(Adafruit_SSD1306 * display){
            display->setCursor(0,30);
            display->print("Level: ");
            display->println(InputValues[0]);
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
                    return "Level";
                default:
                    return "Error";
            }
        }

        float getInputValue(uint32_t index){
            return InputValues[index];
        }

        void setInputValue(uint32_t index, float value){
            InputValues[index] = value;
        }
};