#include <FastLED.h>

#define DATA_PIN 21   // Data pin connected to the LED strip
#define NUM_LEDS 30   // Replace with the number of LEDs in your strip

CRGB leds[NUM_LEDS];

// Brightness levels
uint8_t brightnessLevels[] = {255, 255, 255}; // Adjust as needed (0 to 255)
int currentBrightnessIndex = 0;             // Start with the first brightness level

void setup() {
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  // Set all LEDs to white with the current brightness
  FastLED.setBrightness(brightnessLevels[currentBrightnessIndex]);
  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.show();

  // Wait for 2 seconds
  delay(2000);

  // Cycle to the next brightness level
  currentBrightnessIndex++;
  if (currentBrightnessIndex >= sizeof(brightnessLevels) / sizeof(brightnessLevels[0])) {
    currentBrightnessIndex = 0; // Wrap around to the first brightness level
  }
}