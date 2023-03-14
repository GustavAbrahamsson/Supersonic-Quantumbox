
#include <string>

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
        uint32_t InputValues[4] = {4500, 1000, 1000, 1000}; // something something input
        String inputNames[4] = {"CLIP", "SMTH", "SQRN", "EVEN"};

        double clip_apex = 0.4*max_amplitude;
        double hard_level = 0.4*max_amplitude;
        float clip_smoothness = 0.3f;
        float squareness = 0.5f;
        float evenness = -1.0f;
        
        // Calculated parameters from inputs, adjusted for squareness
        double soft_clip = clip_apex * (1 - clip_smoothness / 2) * (1 - squareness);
        double hard_clip = clip_apex * (1 + clip_smoothness / 2) * (1 - squareness);
        double soft_level = soft_clip / (1 - squareness);

        // Draw
        Adafruit_SSD1306* disp;
        uint8_t box_size_x = 64;
        uint8_t box_size_y = 64;
        uint8_t box_x0 = 63;
        uint8_t box_y0 = 0;

        
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


    public:
        int32_t DSP(int32_t sample){
            
            //int32_t clip = ((int32_t) InputValues[0])*((INT32_MAX)/1024); 
        
            return sample;
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

            for (int i = 0; i++; i < 1){
                //display->setCursor(0, 10*(i+1));
                //display->print(inputNames[i]);
                //display->print(InputValues[i]);
            }
            drawLine_box(0, 32, 63, 32); // x-axis
            drawLine_box(32, 0, 32, 63); // y-axis

            display->drawRect(box_x0, box_y0, box_size_x, box_size_y, 1); // box

            //drawLine_sample(0, 0, INT32_MAX / 2, INT32_MAX / 2);

            drawLine_sample(0, 0, soft_clip, soft_level);
            drawLine_sample(0, 0, -soft_clip, -soft_level);
        }


        void updateCurve(int32_t new_clip_apex, int32_t new_hard_level, float new_clip_smoothness, float new_squareness, float new_evenness){
            // Update
            clip_apex = new_clip_apex;
            hard_level = new_hard_level;
            clip_smoothness = new_clip_smoothness;
            squareness = new_squareness;
            evenness = new_evenness;
            
            // Calculated parameters from inputs
            soft_clip = clip_apex * (1 - clip_smoothness / 2);
            hard_clip = clip_apex * (1 + clip_smoothness / 2);
            soft_level = soft_clip;

            // Adjust for squareness
            soft_clip *= (1 - squareness);
            clip_apex *= (1 - squareness);
            hard_clip *= (1 - squareness);
        
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

        uint32_t getInputValue(uint32_t index){
            return InputValues[index];
        }

        void setInputValue(uint32_t index, uint32_t value){
            InputValues[index] = value;
        }



};















