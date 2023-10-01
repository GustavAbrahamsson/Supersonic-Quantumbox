#include "GenericEffect.h"

// Limits the signal (hard clip) 
class NoiseGate : public GenericEffect{
    private:
        String name = "NoiseGate";
        float InputValues[1] = {0.0f};
        float sampleTracker = 0.0f;
        const float trackingConstant = 0.01f;
        const float potScaling = 0.1f;
    public:

        float DSP(float sample){
            // Pass through
            if(this->pass)
                return sample;

            // Try to track the signal and use it as some sort of signal average
            sampleTracker += (abs(sample) - sampleTracker) * trackingConstant;

            float threshold = InputValues[0] * potScaling; 

            // If "signal average" is above the threshold, let it through
            if(sampleTracker > threshold){
                return sample;
            }
            // If it is negligible, "round down" to zero
            else{
                return 0;
            }
        }

        void Draw(Adafruit_SSD1306 * display){
            display->setCursor(0,30);
            display->print("Threshold: ");
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
                    return "Threshold";
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