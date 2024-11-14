#include <FastLED.h>

// Define LED strip parameters
#define LED_PIN     21  // Data pin for the LED strip
#define NUM_LEDS    30  // Number of LEDs on your strip
#define BRIGHTNESS  128
#define LED_TYPE    WS2812B
#define COLOR_ORDER RGB

CRGB leds[NUM_LEDS];

int brightness = 128; // Starting brightness (range 0 - 255)

void setup() {
    // Initialize the LED strip
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(brightness);

    // Set up any additional pins or controllers here
    Serial.begin(115200);
}

void loop() {
    // Example: set all LEDs to blue
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();

    // Example brightness control with serial input for testing
    if (Serial.available()) {
        char command = Serial.read();
        if (command == '+') {
            brightness = min(255, brightness + 15);  // Increase brightness
        } else if (command == '-') {
            brightness = max(0, brightness - 15);    // Decrease brightness
        }
        FastLED.setBrightness(brightness);
        FastLED.show();
        Serial.println(brightness);
    }

    delay(100); // Adjust delay as needed
}