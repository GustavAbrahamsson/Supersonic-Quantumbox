#include <Arduino.h>
#include <NeoPixelBus.h>
#include <ESP32Encoder.h>
#include "driver/i2s.h"
//#include "dsps_fft2r.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#pragma GCC diagnostic warning "-Wpedantic" // Enable pedantic warnings for own libraries
#pragma GCC optimize("O3")                  // Optimize for speed
#pragma GCC optimize("fast-math")           // Enable fast math (Lower accuracy)

#include <MenuHelper.h>

#include <AudioBuffer.h>
#include <GenericEffect.h>

#include <Clip.h>
#include <Meter.h>
#include <DelayEffect.h>
#include <Saturation.h>
#include <Gain.h>
#include <Square.h>


// --------------CONFIG-----------------------
// comment out to disable

#define USE_OLED
#define USE_ENCODER
#define USE_PIXELS
//#define USE_LEDS
#define USE_TRUE_BP


// Set to true to enable pot
bool POTS_ENABLED[6] = {true, true, true, false, false, false};

// ----------------Pins-----------------------
#define POT1      GPIO_NUM_1
#define POT2      GPIO_NUM_2
#define POT3      GPIO_NUM_3
#define POT4      GPIO_NUM_4
#define PIXELPIN  GPIO_NUM_5
#define SCL       GPIO_NUM_6
#define SDA       GPIO_NUM_7
#define POT6      GPIO_NUM_8
#define POT5      GPIO_NUM_9
#define ENC_B     GPIO_NUM_10
#define ENC_A     GPIO_NUM_11
#define ENC_SW    GPIO_NUM_12
#define LED1      GPIO_NUM_13
#define LED2      GPIO_NUM_14
#define BCLK      GPIO_NUM_15
#define DIN       GPIO_NUM_16
#define DOUT      GPIO_NUM_17
#define LRCLK     GPIO_NUM_18
#define TRUE_BP   GPIO_NUM_37 // Used on Gustav's pedal

// ----------Global variables-----------------

// Effects
Meter MaxInputMeter;
Clip outputClip;
DelayEffect delayEffect;
Meter maxOutputMeter;
Saturation saturationEffect;
Square squareEffect;

GenericEffect * effects[] = {
  &MaxInputMeter,
  &saturationEffect,
  &squareEffect,
  &maxOutputMeter,
  //&delayEffect, 
  &outputClip
  };

const uint32_t numEffects = sizeof(effects)/sizeof(effects[0]);

// Bindings for each pot 
// potToEffect[potNum] = effectNum
int8_t potToEffect[] = {1, 1, 1, 1, 1, 1};

// DSP variables
#define I2S_NUM   I2S_NUM_0
#define BUFFSIZE  128
#define F_MUL (float)INT32_MAX
#define F_DIV (float)(1.0f/INT32_MAX)

int32_t i2s_write_buff[BUFFSIZE*2];
int32_t i2s_read_buff[BUFFSIZE*2];
float avgDspTime = 0;

// True: true bypass, the signal is skipping the pedal
// False: the signal is passing through the pedal
volatile bool trueBypass = false;

// Neopixel
#ifdef USE_PIXELS
  int pixelCount = 2;
  RgbColor pixelColors[] = {RgbColor(64,0,0), RgbColor(0,0,0)};
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(pixelCount, PIXELPIN);
#endif

// Encoder
#ifdef USE_ENCODER
  ESP32Encoder encoder;
  bool encoderButton = false;
  int32_t encoderCount = 0;
#endif

// potentiometers
float pots[6];
#define F_POTS (1.0f/8196.0f)
gpio_num_t potPins[6] = {POT1, POT2, POT3, POT4, POT5, POT6};

//deadband for pots
bool potsChanged[6] = {false, false, false, false, false, false};
uint32_t potsLastChanged[6] = {0, 0, 0, 0, 0, 0};
#define POTS_CHANGED_THRESHOLD 0.03f

// OLED
#ifdef USE_OLED
  TwoWire OledWire = TwoWire(0);
  Adafruit_SSD1306 display(128, 64, &OledWire, -1);

  PedalContext ctx = {
    effects,
    numEffects,
    &display,
    0,
    0
  };

  MenuHelper menuHelper = NULL;
#endif

// Macro for debug output during startup
#ifdef USE_OLED
  #define debugPrint(x) {display.println(x); display.display();}
#else
  #define debugPrint(x) {Serial.println(x);}
#endif

// LED filaments
#ifdef USE_LEDS
  int ledBrightness[] = {0, 0};
#endif

// Is executed every time the switch changes
volatile uint32_t lastSwitchChange = 0;
#define DEBOUNCE_TIME_MS 20
void IRAM_ATTR switchISR() {
    //display.println("MAIN SWITCH EVENT2");
    //Serial.println("MAIN SWITCH EVENT 2");

    
    // Avoid bouncing by only allowing switch-changes at least DEBOUNCE_TIME_MS apart
    if (millis() - lastSwitchChange > DEBOUNCE_TIME_MS){
      lastSwitchChange = millis();
      trueBypass = digitalRead(TRUE_BP);

      // write to neopixels
      #ifdef USE_PIXELS
        // Red for "active"
        if (trueBypass) strip.SetPixelColor(0, pixelColors[0]);
        else strip.SetPixelColor(0, RgbColor(0,0,0));
        
        strip.SetPixelColor(1, pixelColors[1]);
        strip.Show();
      #endif
    
  }
}


// --------------Functions--------------------

void install_i2s(){
  // Don't need these warnings, this code is from espressif docs
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpedantic"
  #pragma GCC diagnostic ignored "-Wmissing-field-initializers"

  // I2S
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX), // Both TX and RX in master mode
    .sample_rate = 48000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, 
    .dma_buf_count = 4,
    .dma_buf_len = BUFFSIZE,
    .use_apll = true,
    .tx_desc_auto_clear = true,
    .mclk_multiple = I2S_MCLK_MULTIPLE_256,
  };

  i2s_pin_config_t pin_config = {
    .mck_io_num = 0,
    .bck_io_num = BCLK,
    .ws_io_num = LRCLK,
    .data_out_num = DOUT,
    .data_in_num = DIN
  };

  #pragma GCC diagnostic pop

  ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL));
  ESP_ERROR_CHECK(i2s_set_pin(I2S_NUM, &pin_config));
}

void AudioTask(void *pvParameters){

  // Wait for input buffer to fill up
  delay(1000);

  while(1){

    // Read from I2S
    size_t bytes_written;
    ESP_ERROR_CHECK(i2s_read(I2S_NUM, (char *) i2s_read_buff, BUFFSIZE*2*sizeof(int32_t), &bytes_written, portMAX_DELAY));
    uint32_t startTime = micros();

    // Send to a chunk to DSP function, one sample at a time
    for (int i = 0; i < BUFFSIZE; i++)
    {
      //Save current sample
      float s = ((float) i2s_read_buff[i * 2])*F_DIV;

      // Call DSP function for modified sample
      for (int e = 0; e < numEffects; e++)
      {
        s = effects[e]->DSP(s);
      }

      int32_t si = s*F_MUL;
      //Write sample to I2S buffer
      i2s_write_buff[i * 2] = si;
      i2s_write_buff[i * 2 + 1] = si;
    }

    // Calculate DSP time
    uint32_t DSPTime = micros() - startTime;
    float DSPpct = DSPTime*(48000.0f/(BUFFSIZE*1000000.0f));
    avgDspTime = (avgDspTime * 0.95f) + (DSPpct * 0.05f);

    // Write to I2S
    ESP_ERROR_CHECK(i2s_write(I2S_NUM, (const char*) &i2s_write_buff, BUFFSIZE *2* sizeof(int32_t), &bytes_written, portMAX_DELAY));
  }
}

void PeripheralTask(void *pvParameters){
  TickType_t LastWakeTime = xTaskGetTickCount();
  float taskFrequency = 5; // Hz
  float taskMillis = 1000.0 / taskFrequency;
  const TickType_t xFrequency = pdMS_TO_TICKS(taskMillis);
  
  #ifdef USE_OLED
    //uint32_t maxRam = ESP.getPsramSize();
    
    int32_t lastEncoderCount = 0;
    uint32_t encoderButtonCount = 0;
    menuHelper = MenuHelper(&ctx);
    bool buttonHeld = false;
  #endif

  while(1){

    // send pot values to effects
    for(int i = 0; i<6; i++){
      if(POTS_ENABLED[i]){
        int8_t effectNum = potToEffect[i];
        if(effectNum != -1 && potsChanged[i] && effects[effectNum]->getNumInputs() > i)
          effects[effectNum]->setInputValue(i, pots[i]);        
      }
    }
    
    // Read encoder
    #ifdef USE_ENCODER
      encoderCount = encoder.getCount()/2;
      encoderButton = !digitalRead(ENC_SW);
      encoderButtonCount += encoderButton;
    #endif

    // write to OLED
    #ifdef USE_OLED

      if(!encoderButton && encoderButtonCount > 0 && encoderButtonCount < 5 && !buttonHeld){
        menuHelper.HandleInput(MENU_PRESS);
        encoderButtonCount = 0;
      }else if (encoderButton && encoderButtonCount >= 5 && !buttonHeld){
        menuHelper.HandleInput(MENU_HOLD);
        buttonHeld = true;
      }else if (!encoderButton && buttonHeld){
        encoderButtonCount = 0;
        buttonHeld = false;
      }else if (encoderCount > lastEncoderCount){
        menuHelper.HandleInput(MENU_LEFT);
        lastEncoderCount += 1;
      }else if (encoderCount < lastEncoderCount){
        menuHelper.HandleInput(MENU_RIGHT);
        lastEncoderCount -= 1;
      }else{
        menuHelper.UpdateDisplay();
      }

    #endif

    //write to LED filaments
    #ifdef USE_LEDS
      ledcWrite(0, ledBrightness[0]);
      ledcWrite(1, ledBrightness[1]);
    #endif

    // vTaskDelayUntil(&LastWakeTime, xFrequency);
    vTaskDelay(xFrequency);

  }
}

void potLoop(void *pvParameters){
  TickType_t LastWakeTime = xTaskGetTickCount();
  uint16_t taskFrequency = 100; // Hz
  uint16_t taskMillis = 1000.0 / taskFrequency;
  const TickType_t xFrequency = pdMS_TO_TICKS(taskMillis);

  // FIR filter for smoothing pots
  while(1){
    for (int i = 0; i < 6; i++)
    {
      if(POTS_ENABLED[i]){
        float oldVal = pots[i];
        float newVal = analogRead(potPins[i])*F_POTS;

        // Linear for a short amount of time after a change
        if (potsChanged[i]){
          if (millis() - potsLastChanged[i] < 500){
            pots[i] = newVal;
          } else {
            potsChanged[i] = false;
          }
        }

        // If the pot has changed, update the value, reset time
        if(abs(newVal - oldVal) > POTS_CHANGED_THRESHOLD){
          potsChanged[i] = true;
          pots[i] = newVal;
          potToEffect[i] = ctx.CurrentEffectNum;
          potsLastChanged[i] = millis();
        }

      }
    }

    vTaskDelayUntil(&LastWakeTime, xFrequency);
  }
}



void setup() {

  delay(3000); // If it crashes after the delay, there is time to upload

  // put your setup code here, to run once:¨
  //#ifndef USE_OLED
    Serial.begin(115200);
    Serial.setDebugOutput(true);
  //#endif

  // Initialize OLED
  #ifdef USE_OLED
    OledWire.setPins(SDA, SCL);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, true);
    display.clearDisplay();
    display.setTextColor(1);
    display.println("SUPERSONIC QUANTUMBOX");
    display.display();
    display.setFont(&Picopixel);

    debugPrint("OLED initialized");
  #endif

  // Initialize the strip
  #ifdef USE_PIXELS
    strip.Begin();
    strip.Show();
    debugPrint("Strip initialized");
  #endif

  // Initialize the encoder
  #ifdef USE_ENCODER
    encoder.attachHalfQuad(ENC_A, ENC_B);
    encoder.clearCount();
    pinMode(ENC_SW, INPUT_PULLUP);
    debugPrint("Encoder initialized");
  #endif

  // Initialize I2S
  install_i2s();
  debugPrint("I2S initialized");

  delay(1000);

  // If the state of the switch (true bypass) is measured at pin 'TRUE_BP'
  #ifdef USE_TRUE_BP
    pinMode(TRUE_BP, INPUT);
    trueBypass = digitalRead(TRUE_BP);
    attachInterrupt(digitalPinToInterrupt(TRUE_BP), switchISR, CHANGE);
    debugPrint("ISR initialized");
  #endif

  // Initialize the pots
  analogSetAttenuation(ADC_11db);
  for (int i = 0; i < 6; i++)
  {
    if(POTS_ENABLED[i]){
      pinMode(potPins[i], INPUT);
      pots[i] = analogRead(potPins[i]);
    }
  }
  debugPrint("Pots initialized");

  // Initialize LED filament
  #ifdef USE_LEDS
    pinMode(LED1, OUTPUT);
    ledcAttachPin(LED1, 0);
    ledcSetup(0, 1000, 8);
    ledcWrite(0, 0);

    pinMode(LED2, OUTPUT);
    ledcAttachPin(LED2, 1);
    ledcSetup(1, 1000, 8);
    ledcWrite(1, 0);
    debugPrint("LEDs initialized");
  #endif

  // Start the audio task
  psramInit();
  debugPrint("PSRAM initialized");

  // Initialize effects
  for(int i = 0; i < numEffects; i++){
    effects[i]->init();
  }
  
  delay(1000);
  // Start audio task
  xTaskCreate(AudioTask, "AudioTask", 10000, NULL, 10, NULL);
  //display.clearDisplay();
  //display.setCursor(0, 4);
  debugPrint("Audio task started");

  delay(1000);

  // Start pot task
  xTaskCreate(potLoop, "potLoop", 10000, NULL, 1, NULL);
  debugPrint("PotLoop started");

  delay(1000);
  // Start peripheral task
  // TODO: Split updating display and handling input into two tasks
  xTaskCreate(PeripheralTask, "PeripheralTask", 10000, NULL, 1, NULL);
  // Do not use display after this point, it is used by the peripheral task

  delay(1000);

  vTaskStartScheduler();
}

void loop() {
  delay(50000);
}
