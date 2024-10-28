#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUM_LEDS 60 // Number of LEDs


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show();
}

void loop() {

  setStripBrightness(50);
  delay(1000);

  setStripBrightness(128);
  delay(1000);

  setStripBrightness(200);
  delay(1000);
}


void setStripBrightness(uint8_t brightness) {
  strip.setBrightness(brightness);


  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 250, 250, 250)); // White (R, G, B, W)
  }

  strip.show();
}