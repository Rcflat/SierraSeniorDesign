#include <ESP32Servo.h>  // Include the ESP32 Servo library to control servo motors

// Define servo objects for pan and tilt movements
Servo panServo;
Servo tiltServo;

// Define pin numbers for the pan and tilt servos
int panPin = 33;
int tiltPin = 32;

void setup() {
  // Attach the servos to their respective pins
  panServo.attach(panPin);  // Attach pan servo to pin 33
  tiltServo.attach(tiltPin);  // Attach tilt servo to pin 32
}

void loop() {
    panServo.write(1500);
    tiltServo.write(1500);

    // Small delay to stabilize the servos
    delay(20);
}