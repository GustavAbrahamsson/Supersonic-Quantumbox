#include "GenericEffect.h"

class Clip : public GenericEffect{
    private:
        String name = "Clip";
        uint32_t InputValues[1] = {0};

    public:

        int32_t DSP(AudioBuffer<int32_t> * input, AudioBuffer<int32_t> * output){
            // Clip me baby!
            int32_t in = input->read(0);
            int32_t clip = ((int32_t) InputValues[0])*((1<<24)/8192); 
        
            if(in > clip){
                return clip;
            }
            if(in < -clip){
                return -clip;
            }
        
            return in;
        }

        void Draw(Adafruit_SSD1306 * display){
            display->clearDisplay();
            display->setCursor(0,30);
            display->println(name);
            display->print("Level: ");
            display->println(InputValues[0]);
            display->display();
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