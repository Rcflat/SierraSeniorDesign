#include <FastLED.h>

#define PIN 21                // Pin where the LED strip is connected
#define NUM_LEDS 60           // Number of LEDs in the strip
#define LED_TYPE WS2812       // Use WS2812 chipset as WS2814 is not natively supported in FastLED
#define COLOR_ORDER GRB       // Color order for WS2814 (Green, Red, Blue)

CRGB leds[NUM_LEDS];         // Create an array to hold the LED data

void setup() {
  FastLED.addLeds<LED_TYPE, PIN, COLOR_ORDER>(leds, NUM_LEDS); // Initialize the LEDs
  FastLED.setBrightness(128);  // Set the initial brightness to 128 (50%)
}

void loop() {
  // Set all LEDs to white (using RGB and manually controlling white with the W channel)
  fill_solid(leds, NUM_LEDS, CRGB(255, 255, 255)); // Full white with RGB
  // Optionally, adjust the white channel separately if needed:
  // leds[0] = CRGB(255, 255, 255);  // White without RGB mix

  FastLED.show();
  delay(1000); // Wait for 1 second

  // Set all LEDs to red with some white
  fill_solid(leds, NUM_LEDS, CRGB(255, 0, 0)); // Red
  // Manually add white:
  leds[0] = CRGB(255, 0, 0); // Red with white manually adjusted
  FastLED.show();
  delay(1000); // Wait for 1 second

  // Set all LEDs to blue with white
  fill_solid(leds, NUM_LEDS, CRGB(0, 0, 255)); // Blue
  // Optionally, adjust the white channel manually
  leds[0] = CRGB(0, 0, 255); // Blue with white
  FastLED.show();
  delay(1000); // Wait for 1 second
}

