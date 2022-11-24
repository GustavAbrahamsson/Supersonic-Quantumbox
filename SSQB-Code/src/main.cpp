#include <Arduino.h>
#include <NeoPixelBus.h>
#include <ESP32Encoder.h>
#include "driver/i2s.h"
#include "driver/gpio.h"

// Pins
#define POT1 1
#define POT2 2
#define POT3 3
#define POT4 4
#define PIXELPIN 5
#define SCL 6
#define SDA 7
#define POT6 8
#define POT5 9
#define ENC_B 10
#define ENC_A 11
#define ENC_SW 12
#define LED1 13
#define LED2 14
#define BCLK 15
#define DIN 16
#define DOUT 17
#define LRCLK 18

// Global variables

// Neopixel
int pixelCount = 2;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(pixelCount, PIXELPIN);

// Encoder
ESP32Encoder encoder;

void install_i2s(){
  // I2S
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX), // Both TX and RX in master mode
    .sample_rate = 48000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_24BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Interrupt level 1, default 0
    .dma_buf_count = 2,
    .dma_buf_len = 128,
    .use_apll = true,
    .mclk_multiple = I2S_MCLK_MULTIPLE_256
  };

  i2s_pin_config_t pin_config = {
    .mck_io_num = 0,
    .bck_io_num = BCLK,
    .ws_io_num = LRCLK,
    .data_out_num = DOUT,
    .data_in_num = DIN
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Initialize the strip
  strip.Begin();
  strip.Show();

  // Initialize the encoder
  encoder.attachHalfQuad(ENC_A, ENC_B);
  encoder.clearCount();

  // Initialize I2S
  install_i2s();

}

void loop() {
  // put your main code here, to run repeatedly:
  // set whole strip to red
  int h = encoder.getCount();
  strip.ClearTo(HslColor(h/100.0, 1, 0.5));
  strip.Show();
  delay(10);
}