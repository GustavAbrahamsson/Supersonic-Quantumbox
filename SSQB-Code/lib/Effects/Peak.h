#include "GenericEffect.h"

// IIR peaking filter
class Peak : public GenericEffect{
    private:
        String name = "Peak";
        float InputValues[2] = {0.1f, 0.5f};
        String inputNames[2] = {"Freq", "Q",};
        float beta = 0;
        float num[3] = {0,0,0};
        float denom[3] = {1,0,0};

        float x[2] = {0,0};
        float y[2] = {0,0};

        void calculateCoeffs(){
            // From Introduction to Signal Processing, Orfanidis pg 520
            // http://eceweb1.rutgers.edu/~orfanidi/intro2sp/2e/orfanidis-isp2e-1up.pdf 

            beta = 0.1f/InputValues[1];
            float w0 = InputValues[0]*PI*0.1f;

            float b = 1.0f/(1.0f + beta);

            num[0] = (1-b);
            num[2] = -(1-b);

            denom[1] = -2*cos(w0)*b;
            denom[2] = 2*b-1;

        }

    public:

        float DSP(float sample){
            // Pass through
            if (this->pass)
                return sample;

            // Shift x and y buffers
            float x2 = x[1];
            x[1] = x[0];
            x[0] = sample;

            float y2 = y[1];
            y[1] = y[0];

            // Biquad direct form
            y[0] = num[0]*sample + num[2]*x2 - denom[1]*y[1] - denom[2]*y2;

            return y[0];
        }

        void Draw(Adafruit_SSD1306 * display){
            display->setCursor(0,30);
        }

        void init(){
            calculateCoeffs();
        }

        String getName(){
            return name;
        }

        uint32_t getNumInputs(){
            return 2;
        }

        String getInputName(uint32_t index){
            switch(index){
                case 0:
                    return inputNames[0];
                case 1:
                    return inputNames[1];
                default:
                    return "Error";
            }
        }

        float getInputValue(uint32_t index){
            if(index > 1)
                return 0.0f;
            return InputValues[index];
        }

        void setInputValue(uint32_t index, float value){
            if(index > 1)
                return;
            InputValues[index] = value;

            calculateCoeffs();
        }
};