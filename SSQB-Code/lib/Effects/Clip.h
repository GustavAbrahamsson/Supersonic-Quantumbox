#include "GenericEffect.h"

class Clip : public GenericEffect{
    private:
        String name = "Clip";
        uint32_t InputValues[1] = {4500};

    public:

        int32_t DSP(int32_t sample){
            // Clip me baby!
            int32_t clip = ((int32_t) InputValues[0])*((INT32_MAX)/1024); 
        
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

        uint32_t getInputValue(uint32_t index){
            return InputValues[index];
        }

        void setInputValue(uint32_t index, uint32_t value){
            InputValues[index] = value;
        }
};