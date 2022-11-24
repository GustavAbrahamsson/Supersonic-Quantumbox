#include <Arduino.h>
#include <NeoPixelBus.h>
#include <ESP32Encoder.h>

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


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Initialize the strip
  strip.Begin();
  strip.Show();

  // Initialize the encoder
  encoder.attachHalfQuad(ENC_A, ENC_B);
  encoder.clearCount();

}

void loop() {
  // put your main code here, to run repeatedly:
  // set whole strip to red
  int h = encoder.getCount();
  strip.ClearTo(HslColor(h/100.0, 0, 1));
  strip.Show();
  delay(10);
}