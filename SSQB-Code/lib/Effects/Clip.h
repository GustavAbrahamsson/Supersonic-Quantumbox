#include "GenericEffect.h"

// Limits the signal (hard clip) 
class Clip : public GenericEffect{
    private:
        String name = "Clip";
        float InputValues[1] = {1.0f};

    public:

        float DSP(float sample){
            // Pass through
            if(this->pass)
                return sample;

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
            display->print(InputValues[0]*100.0f);
            display->println("%");
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
            if (index > 0)
                return 0.0f;
            return InputValues[index];
        }

        void setInputValue(uint32_t index, float value){
            if (index > 0)
                return;
            InputValues[index] = value;
        }
};