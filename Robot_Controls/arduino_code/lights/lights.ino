#include <FastLED.h>

#define LED_PIN     21  // Data pin for the LED strip
#define NUM_LEDS    30  // Number of LEDs on your strip
#define LED_TYPE    WS2812B  // Type of LED
#define COLOR_ORDER GRB  // Color order (GRB for WS2812B)

CRGB leds[NUM_LEDS];

// Brightness levels to cycle through (0 to 255)
int brightnessLevels[] = {50, 128, 255};
int currentBrightnessIndex = 0;  // Start with the first brightness level

unsigned long lastChangeTime = 0;  // Timer for cycling brightness
unsigned long cycleInterval = 2000;  // Change brightness every 2 seconds

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);  // Initialize the LED strip
  FastLED.show();  // Initialize the strip to off
}

void loop() {
  // Check if it's time to change brightness
  if (millis() - lastChangeTime >= cycleInterval) {
    lastChangeTime = millis();  // Reset the timer

    // Set all LEDs to white with the current brightness
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::White;  // Set each LED to white
      leds[i].fadeToBlackBy(255 - brightnessLevels[currentBrightnessIndex]);  // Apply brightness level
    }

    // Update the strip to apply changes
    FastLED.show();  

    // Move to the next brightness level
    currentBrightnessIndex++;
    if (currentBrightnessIndex >= 3) {
      currentBrightnessIndex = 0;  // Reset to the first brightness level
    }
  }
}