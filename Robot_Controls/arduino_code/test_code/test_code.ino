#include <ESP32Servo.h>  // Include the ESP32 Servo library to control servo motors

// Define servo objects for pan and tilt movements
char motor1SpeedChar[20];
char motor2SpeedChar[20];

void SendSabertoothCommand(int motor1Speed, int motor2Speed) {

  sprintf(motor1SpeedChar, "M1: %d \r\n", motor1Speed);
  Serial2.print(motor1SpeedChar);
  sprintf(motor2SpeedChar, "M2: %d \r\n", motor2Speed);
  Serial2.print(motor2SpeedChar);
}

void setup() {
    int leftMotorValue = 1500;
    int rightMotorValue = 1500;
}

void loop() {
    SendSabertoothCommand(leftMotorValue, rightMotorValue);
}