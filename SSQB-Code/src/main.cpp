#include <Arduino.h>
#include <NeoPixelBus.h>
#include <ESP32Encoder.h>
#include "driver/i2s.h"
#include "dsps_fft2r.h"

// Pins
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

// Global variables

#define I2S_NUM   I2S_NUM_0
#define BUFFSIZE  128

int16_t i2s_write_buff[BUFFSIZE*2];
int16_t i2s_read_buff[BUFFSIZE*2];
uint32_t n = 0;
float avgDspTime = 0;

// Neopixel
int pixelCount = 2;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(pixelCount, PIXELPIN);

// Encoder
ESP32Encoder encoder;

void DSP(int16_t * InBuff, int16_t * OutBuff, size_t length)
{
  // Here you can do your DSP stuff
  // InBuff contains the input data
  // OutBuff is the output buffer
  // length is the number of frames
  // A frame contains one left and one right sample

  for (int i = 0; i < BUFFSIZE; i++)
    {
      int16_t s = 5000 * sin(440.0 * 2 * PI * n / 48000.0);
      i2s_write_buff[i*2] = s;
      i2s_write_buff[i*2+1] = s;
      n++;
    }
}


void install_i2s(){
  // I2S
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX), // Both TX and RX in master mode
    .sample_rate = 48000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .dma_buf_count = 4,
    .dma_buf_len = BUFFSIZE,
    .use_apll = true,
    .fixed_mclk = 12288000, 
  };

  i2s_pin_config_t pin_config = {
    .mck_io_num = 0,
    .bck_io_num = BCLK,
    .ws_io_num = LRCLK,
    .data_out_num = DOUT,
    .data_in_num = DIN
  };

  esp_err_t e = i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  if (e != ESP_OK) {
    Serial.println("I2S driver install failed");
  }
  e = i2s_set_pin(I2S_NUM, &pin_config);
  if (e != ESP_OK) {
    Serial.println("I2S pin config failed");
  }
}

void AudioTask(void *pvParameters){
  
  // Wait for input buffer to fill up
  delay(1000);

  uint32_t n = 0;
  while(1){

    // Read from I2S    
    size_t bytes_written;
    ESP_ERROR_CHECK(i2s_read(I2S_NUM, (char *) i2s_read_buff, BUFFSIZE*2, &bytes_written, portMAX_DELAY));
    uint32_t startTime = micros();

    // Send to DSP function
    DSP(i2s_read_buff, i2s_write_buff, BUFFSIZE);
    
    uint32_t DSPTime = micros() - startTime;
    float DSPpct = DSPTime*(48000.0/(BUFFSIZE*1000000.0));

    avgDspTime = (avgDspTime * 0.95) + (DSPpct * 0.05);

    // Write to I2S
    bytes_written;
    ESP_ERROR_CHECK(i2s_write(I2S_NUM, (const char*) &i2s_write_buff, BUFFSIZE *2* sizeof(int16_t), &bytes_written, portMAX_DELAY));
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Starting up");

  // Initialize the strip
  strip.Begin();
  strip.Show();
  Serial.println("Strip initialized");

  // Initialize the encoder
  encoder.attachHalfQuad(ENC_A, ENC_B);
  encoder.clearCount();
  Serial.println("Encoder initialized");

  // Initialize I2S
  install_i2s();
  Serial.println("I2S initialized");

  // Start the audio task
  xTaskCreate(AudioTask, "AudioTask", 10000, NULL, 10, NULL);

}

void loop() {
  // write sine wave to I2S
  Serial.print("DSP CPU %: ");
  Serial.println(avgDspTime, 4);
  delay(5000);
}
