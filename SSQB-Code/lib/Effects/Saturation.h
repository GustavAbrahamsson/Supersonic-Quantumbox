
#include <string>
#include "math.h"

// Creates a saturation effect following something similar to:

// .............................................................#..................................................................
// ....................................MAX POSSIBLE OUTPUT...#######...............................................................
// .............................................................#............ Clipping apex........................................
// .............................................................#...................I..............................................
// .............................................................#...................I..............................................
// .............................................................#...................V..............................................
// ...............................................hard_level...###..................*.......####################################...
// .............................................................#.......................####.......................................
// .............................................................#....................###...........................................
// .............................................................#..................##..............................................
// .............................................................#................##................................................
// ...............................................soft_level...###..............#..................................................
// .............................................................#..............#...................................................
// .............................................................#.............#....................................................
// .............................................................#............#.....................................................
// .............................................................#...........#......................................................
// .............................................................#..........#.......................................................
// .............................................................#.........#........................................................
// .............................................................#........#.........................................................
// .............................................................#.......#..........................................................
// .............................................................#......#...........................................................
// .............................................................#.....#............................................................
// .............................................................#....#.............................................................
// .............................................................#...#..............................................................
// .............................................................#..#...............................................................
// .............................................................#.#................................................................
// .............................................................##..............#...*.......#......................................
// #################################################################################*###########################################...
// ............................................................##...............#...*.......#......................................
// ...........................................................#.#...........soft_clip...hard_clip..................................
// ..........................................................#..#..................................................................
// .........................................................#...#..................................................................
// ......................................................###....#..................................................................
// ....................................................##.......#..................................................................
// ..................................................##.........#..................................................................
// ................................................##...........#..................................................................
// ............................................####.............#..................................................................
// .....................................#######.................#..................................................................
// #####################################.......................###...low_clip......................................................
// .............................................................#..................................................................
// .............................................................#..................................................................

class Saturation : public GenericEffect{
    /*
    Pots:

    Clipping: Input level to engage clipping (clipping apex) (0: no clipping, 10: clip immediately)

    Smoothness: Clip "discontinously" or smoothly begin clipping (0: pointy corner, 10: no straight lines)

    Squareness: Gain before soft clipping (0: 1:1 ratio, 10: Square wave)

    Evenness: How even or odd the curve should be   (0: Completely odd (odd harmonics)),
                                                     5: negative inputs are put to zero (even harmonics with fund. freq),
                                                    10: completely even with double the fundamental frequency (rectification))

    */
    private:
        int32_t max_amplitude = INT32_MAX;
        String name = "Saturation";
        uint8_t numInputs = 4;
        float InputValues[4] = {1.0f, 0.0f, 0.0f, 1.0f}; // something something input
        String inputNames[4] = {"CLIP", "SMTH", "SQRN", "EVEN"};

        // Calculated from inputs
        float gain = 1.0f;
        float neg_gain = 1.0f;
        float soft_end = 1.0f;
        float soft_start = 1.0f;
        float r = 0.0f;
        float s = 0.0f;
        float max_level = 1.0f;
        
        // Draw
        Adafruit_SSD1306* disp;
        #define box_size_x 64
        #define box_size_y 64
        #define box_x0 63
        #define box_y0 0

        #define box_step_x 2.0f/((float) box_size_x)
        #define box_step_y ((float) box_size_y)/2.0f
        
        void drawLine_box(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
            // Constrained to box_size frame
            /*
            Serial.print("drawLine_box ");
            Serial.print("x0, y0, x1, y1 = ");
            Serial.print(x0 + box_x0); Serial.print(" ");
            Serial.print(y0 + box_y0); Serial.print(" ");
            Serial.print(x1 + box_x0); Serial.print(" ");
            Serial.println(y1 + box_y0); Serial.println(" ");
            */
            disp->drawLine(box_x0 + x0, box_y0 + y0, box_x0 + x1, box_y0 + y1, 1);
        }

        void drawPixel_box(uint8_t x, uint8_t y){
            // Constrained to box_size frame
            disp->drawPixel(box_x0 + x, box_y0 + y, 1);
            
        }

        void drawLine_sample(double x0, double y0, double x1, double y1){
            // Constrained to box_size frame, scaled and shifted to receive sample values
            // with frame centered in the origin of the box. y points up, x points right

            // Transform to correct frame

            /*
            Serial.print("drawLine_sample_BEFORE ");
            Serial.print("x0, y0, x1, y1 = ");
            Serial.print(x0); Serial.print(" ");
            Serial.print(y0); Serial.print(" ");
            Serial.print(x1); Serial.print(" ");
            Serial.println(y1); Serial.println(" ");*/

            x0 = box_size_x * ((double)x0 / (double)max_amplitude) + box_size_x / 2;
            y0 =-box_size_y * ((double)y0 / (double)max_amplitude) + box_size_y / 2;
            x1 = box_size_x * ((double)x1 / (double)max_amplitude) + box_size_x / 2;
            y1 =-box_size_y * ((double)y1 / (double)max_amplitude) + box_size_y / 2;
            
            /*
            Serial.print("drawLine_sample ");
            Serial.print("x0, y0, x1, y1 = ");
            Serial.print(x0); Serial.print(" ");
            Serial.print(y0); Serial.print(" ");
            Serial.print(x1); Serial.print(" ");
            Serial.println(y1); Serial.println(" ");*/

            drawLine_box(x0, y0, x1, y1);
        }

        void drawPixel_sample(int32_t x, int32_t y){
            // Constrained to box_size frame, scaled and shifted to receive sample values
            // with frame centered in the origin of the box. y points up, x points right

            // Transform to correct frame
            x = box_size_x * ((double)x / (double)max_amplitude) + box_size_x / 2;
            y =-box_size_y * ((double)y / (double)max_amplitude) + box_size_y / 2;

            drawPixel_box(x, y);
        }

        void updateCurve()
        {
            s = InputValues[1] * (float) M_SQRT2;
            r = s * ((float) M_SQRT2 + 1);

            gain = (1+16*InputValues[2])/InputValues[0];
            neg_gain = InputValues[3];

            soft_start = 1-InputValues[1];
            soft_end = s + 1;
            max_level = InputValues[0];
        }  

    public:
        float DSP(float sample){
            // See https://www.geogebra.org/calculator/ynjbbcu8 for equations
            // "smooth" part is modeled as an arc
            if (this->pass)
                return sample;

            bool neg = false;
            
            float x = sample * gain;

            if(sample < 0)
            {
                x = -x;
                neg = true;
            }

            if(x > soft_end){
                x = 1;
            }else if(x > soft_start){
                x = f_sqrt((r+x-1-s)*(r-x+1+s)) + 1-r;
            }

            x = x * max_level;

            if(neg){
                x= -x*neg_gain;
            }

            return x;
        }

        void Draw(Adafruit_SSD1306* display){
            disp = display; // Save the pointer to use in other functions
            
            display->setCursor(0,15);
            display->print("CLIP: ");
            display->print(InputValues[0]);
            
            display->setCursor(0,25);
            display->print("SMTH: ");
            display->print(InputValues[1]);
            
            display->setCursor(0,35);
            display->print("SQRN: ");
            display->print(InputValues[2]);
            
            display->setCursor(0,45);
            display->print("EVEN: ");
            display->print(InputValues[3]);

            drawLine_box(0, 32, 63, 32); // x-axis
            drawLine_box(32, 0, 32, 63); // y-axis

            display->drawRect(box_x0, box_y0, box_size_x, box_size_y, 1); // box

            uint8_t last_y = 0;

            // feed -1 .. 1 into DSP, display on screen
            for(uint8_t x = 0; x<box_size_x; x++){
                float xf = -1 + x * box_step_x;
                float yf = DSP(xf);
                uint8_t y = 64 - (yf+1) * box_step_y;
                drawPixel_box(x, y);

                if(last_y - y > 1) // if more than one step in y, fill with line
                {
                    drawLine_box(x, last_y-1, x, y+1);
                }
                last_y = y;
            }
        }


        String getName(){
            return name;
        }

        uint32_t getNumInputs(){
            return numInputs;
        }

        String getInputName(uint32_t index){
            switch(index){
                case 0:
                    return inputNames[0];
                case 1:
                    return inputNames[1];
                case 2:
                    return inputNames[2];
                case 3:
                    return inputNames[3];
                default:
                    return "Error";
            }
        }

        float getInputValue(uint32_t index){
            if (index > numInputs-1)
                return 0.0f;
            return InputValues[index];
        }

        void setInputValue(uint32_t index, float value){
            if(index > numInputs-1)
                return;
            InputValues[index] = value;
            updateCurve();              //Update curve parameters
        }



};















