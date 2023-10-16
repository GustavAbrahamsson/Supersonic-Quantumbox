#include "GenericEffect.h"

class DiffuserEffect : public GenericEffect {
    private:
        Diffuser diffuser = Diffuser();
        String name = "Diffuser";
        float InputValues[2] = {0.5f, 0.5f};
        String InputNames[2] = {"Delay", "Alpha"};
        const uint32_t MaxDelay = 2000;

    public:
        float DSP(float sample){
            if (pass) return sample;

            return diffuser.process(sample);
        };

        void Draw(Adafruit_SSD1306 * display){};

        void init(){
            diffuser.init(MaxDelay, MaxDelay * InputValues[0], InputValues[1]);
        };

        String getName(){ return name; };

        uint32_t getNumInputs(){ return 2; };

        String getInputName(uint32_t index){ 
            if (index > 1) return "None";
            return InputNames[index]; 
        };

        float getInputValue(uint32_t index){ 
            if (index > 1) return 0.0f;
            return InputValues[index]; 
        };

        void setInputValue(uint32_t index, float value){ 
            if (index > 1) return;
            InputValues[index] = value;
            switch (index)
            {
            case 0:
                diffuser.setDelay(value * MaxDelay);
                break;
            case 1:
                diffuser.setAlpha(value);
                break;
            default:
                break;
            }
        };
};