#include <Arduino.h>
#include <NeoPixelBus.h>
#include <ESP32Encoder.h>
#include "driver/i2s.h"
#include "dsps_fft2r.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include <Fonts/Picopixel.h>

#include <MenuHelper.h>

#include <AudioBuffer.h>
#include <GenericEffect.h>

#include <Clip.h>
#include <Meter.h>
#include <DelayEffect.h>
#include <Saturation.h>
#include <Gain.h>


// --------------CONFIG-----------------------
// comment out to disable

#define USE_OLED
#define USE_ENCODER
#define USE_PIXELS
#define USE_LEDS

// Set to true to enable pot
bool POTS_ENABLED[6] = {true, true, true, false, false, true};

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

// ----------Global variables-----------------

// Effects
Meter MaxInputMeter;
Clip outputClip;
DelayEffect delayEffect;
Meter maxOutputMeter;
Saturation saturationEffect;

GenericEffect * effects[] = {
  &MaxInputMeter, 
  &delayEffect,
  &maxOutputMeter, 
  &outputClip
  };

const uint32_t numEffects = sizeof(effects)/sizeof(effects[0]);

// Bindings for each pot
PotBinding potBindings[] = {
  {&delayEffect, 0},
  {&delayEffect, 1},
  {&delayEffect, 2},
  {NULL, 0},
  {NULL, 0},
  {NULL, 0}
};

// DSP variables
#define I2S_NUM   I2S_NUM_0
#define BUFFSIZE  128
#define F_MUL (float)INT32_MAX
#define F_DIV (float)(1.0f/INT32_MAX)

int32_t i2s_write_buff[BUFFSIZE*2];
int32_t i2s_read_buff[BUFFSIZE*2];
uint32_t n = 0;
float avgDspTime = 0;

// Neopixel
#ifdef USE_PIXELS
  int pixelCount = 2;
  RgbColor pixelColors[] = {RgbColor(0,0,0), RgbColor(0,0,0)};
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
#define F_POTS 1.0f/8196.0f
gpio_num_t potPins[6] = {POT1, POT2, POT3, POT4, POT5, POT6};

// OLED
#ifdef USE_OLED
  TwoWire OledWire = TwoWire(0);
  Adafruit_SSD1306 display(128, 64, &OledWire, -1);
#endif

// LED filaments
#ifdef USE_LEDS
  int ledBrightness[] = {0, 0};
#endif

// --------------Functions--------------------

void install_i2s(){
  // I2S
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX), // Both TX and RX in master mode
    .sample_rate = 48000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .dma_buf_count = 4,
    .dma_buf_len = BUFFSIZE,
    .use_apll = true,
    .mclk_multiple = I2S_MCLK_MULTIPLE_256,
  };

  i2s_pin_config_t pin_config = {
    .mck_io_num = 0,
    .bck_io_num = BCLK,
    .ws_io_num = LRCLK,
    .data_out_num = DOUT,
    .data_in_num = DIN
  };

  ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL));
  ESP_ERROR_CHECK(i2s_set_pin(I2S_NUM, &pin_config));
}

void AudioTask(void *pvParameters){

  // Wait for input buffer to fill up
  delay(1000);

  uint32_t n = 0;
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

      // increment sample number
      n++;

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
    bytes_written;
    ESP_ERROR_CHECK(i2s_write(I2S_NUM, (const char*) &i2s_write_buff, BUFFSIZE *2* sizeof(int32_t), &bytes_written, portMAX_DELAY));
  }
}

void PeripheralTask(void *pvParameters){

  uint32_t maxRam = ESP.getPsramSize();
  int32_t menuPage = 0;

  while(1){

    //effects[2]->setInputValue(0, pots[0]);
    //effects[2]->setInputValue(1, pots[1]);
    //effects[2]->setInputValue(2, pots[2]);
    
    delayEffect.setInputValue(0, pots[0]);
    delayEffect.setInputValue(1, pots[1]);

    // Read encoder
    #ifdef USE_ENCODER
      encoderCount = encoder.getCount()/2;
      encoderButton = !digitalRead(ENC_SW);
    #endif

    // write to neopixels
    #ifdef USE_PIXELS
      strip.SetPixelColor(0, pixelColors[0]);
      strip.SetPixelColor(1, pixelColors[1]);
      strip.Show();
    #endif

    // write to OLED
    #ifdef USE_OLED
      display.clearDisplay();
      display.setFont(NULL);

      if(encoderCount <= -1)
        menuPage = -1;
      else if(encoderCount > numEffects-1)
        menuPage = numEffects-1;
      else
        menuPage = encoderCount;

      // Menu
      if(menuPage == -1){
        // diagnostics display
        display.setCursor(0,0);
        display.println("Diagnostics");
        display.drawFastHLine(0, 10, 128, 1);
        display.setFont(&Picopixel);
        display.setCursor(0, 20);
        display.println("Encoder: " + String(encoderCount));
        display.println("Button: " + String(encoderButton));
        display.println("Pots: " + String(pots[0]) + ", " + String(pots[1]) + ", " + String(pots[2]));
        display.println("DSP %: " + String(avgDspTime*100));
        display.println("PSRAM used: " + String(maxRam - ESP.getFreePsram()) + " / " + String(maxRam));
      }else{
        effects[menuPage]->Draw(&display);
        display.setCursor(0, 0);
        display.println(effects[menuPage]->getName());
        if(effects[menuPage]->getName() != "Saturation"){
            display.drawFastHLine(0, 10, 128, 1);
        }
        display.setFont(&Picopixel);
        display.setCursor(0, 20);
      }
      
      display.display();
    #endif

    //write to LED filaments
    #ifdef USE_LEDS
      ledcWrite(0, ledBrightness[0]);
      ledcWrite(1, ledBrightness[1]);
    #endif

    // delay
    delay(200);
  }
}

void potLoop(void *pvParameters){
  // FIR filter for smoothing pots
  while(1){
    for (int i = 0; i < 6; i++)
    {
      if(POTS_ENABLED[i]){
        pots[i] = pots[i]*(0.9f) + analogRead(potPins[i])*(0.1f*F_POTS);
      }
    }
    delay(10);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Starting up");

  // Initialize the strip
  #ifdef USE_PIXELS
    strip.Begin();
    strip.Show();
    Serial.println("Strip initialized");
  #endif

  // Initialize the encoder
  #ifdef USE_ENCODER
    encoder.attachHalfQuad(ENC_A, ENC_B);
    encoder.clearCount();
    Serial.println("Encoder initialized");
    pinMode(ENC_SW, INPUT_PULLUP);
  #endif

  // Initialize I2S
  install_i2s();
  Serial.println("I2S initialized");

  // Initialize the pots
  analogSetAttenuation(ADC_11db);
  for (int i = 0; i < 6; i++)
  {
    if(POTS_ENABLED[i]){
      pinMode(potPins[i], INPUT);
      pots[i] = analogRead(potPins[i]);
    }
  }
  Serial.println("Pots initialized");

  // Initialize OLED
  #ifdef USE_OLED
    OledWire.setPins(SDA, SCL);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, true);
    display.clearDisplay();
    display.setTextColor(1);
    display.display();
    Serial.println("OLED initialized");
  #endif

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
    Serial.println("LED initialized");
  #endif

  // Start the audio task
  psramInit();
  Serial.println("PSRAM initialized");
  delayEffect.init();
  
  delay(3000);
  // Start audio task
  xTaskCreate(AudioTask, "AudioTask", 10000, NULL, 10, NULL);
  Serial.println("AudioTask started");
  
  // Start peripheral task
  xTaskCreate(PeripheralTask, "PeripheralTask", 10000, NULL, 2, NULL);
  Serial.println("PeripheralTask started");

  // Start pot task
  xTaskCreate(potLoop, "potLoop", 1000, NULL, 1, NULL);
}

void loop() {
  delay(50000);
}
