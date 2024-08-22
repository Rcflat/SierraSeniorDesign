#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <Servo.h>

// Axis 0: Left Analog Stick (-1 = LEFT, 1 = RIGHT)
// Axis 1: Left Analog Stick (-1 = UP, 1 = DOWN)
// Axis 2: Right Analog Stick (-1 = UP, 1 = DOWN)
// Axis 3: Right Analog Stick (-1 = UP, 1 = DOWN)
// Axis 4: Back Left Trigger (-1 = Released, 1 = Pressed)
// Axis 5: Back Right Trigger (-1 = Released, 1 = Pressed)

// Button 0: X Button (0 = Released, 1 = Pressed)
// Button 1: Circle Button (0 = Released, 1 = Pressed)
// Button 2: Square Button (0 = Released, 1 = Pressed)
// Button 3: Triangle Button (0 = Released, 1 = Pressed)
// Button 4: Select Button (0 = Released, 1 = Pressed)
// Button 5: Playstation Button (0 = Released, 1 = Pressed)
// Button 6: Menu Button (0 = Released, 1 = Pressed)
// Button 9: Back Left Button (0 = Released, 1 = Pressed)
// Button 10: Back Right Button (0 = Released, 1 = Pressed)
// Button 11: DPAD_UP (0 = Released, 1 = Pressed)
// Button 12: DPAD_DOWN (0 = Released, 1 = Pressed)
// Button 13: DPAD_LEFT (0 = Released, 1 = Pressed)
// Button 14: DPAD_RIGHT (0 = Released, 1 = Pressed)
// Button 15: Center Button, Big Panel (0 = Released, 1 = Pressed)

const char* ssid = "NETGEAR32";
const char* password = "breezybreeze113";

// Static IP address configuration
IPAddress ip(192, 168, 1, 50);     // Replace with the desired static IP address
IPAddress gateway(192, 168, 1, 1);       // Replace with your network gateway
IPAddress subnet(255, 255, 255, 0);      // Replace with your network subnet mask
IPAddress dns(8, 8, 8, 8);               // Replaced with your DNS

WiFiUDP udp;
const unsigned int localPort = 8888;

Servo myServoS1;
Servo myServoS2;

void pinMapping(){ // for now only works with controller
  const int outputPinS1 = 2; // PWM output pin for S1
  const int outputPinS2 = 3; // PWM output pin for S2
  const int outputPinS3 = 4; // Servo output pin for S3
  const int outputPinS4 = 5; // Servo output pin for S4
  const int LEDoutputPin = 9; // output pin for LED

  myServoS1.attach(outputPinS1); // Attaching servo to the pin
  myServoS2.attach(outputPinS2); // Attaching servo to the pin
} 

void updateValues(){ // for now only works with controller

  int sensorValueS1 = doc[""];  //analogRead(potPin); // Read potentiometer value (-512-512)
  int sensorValueS2 = doc["axisY"];  //analogRead(potPin); // Read potentiometer value (-512-512)

  // d-pad logic will go on client side in the future but for the preview day I am putting it on the robot to make my life simpler
  int Servo_Ranges[4][2] = {{1450, 1500},{1400, 1550},{1200, 1750},{1000, 1950}}; // desired speed ranges per speed setting
  int dpad_value = doc["dpad"];
  if (dpad_value == 1 && prev_dpad != dpad_value){ // if d-pad top button pressed shift up in range (conditional values currently filler)
    if (speed_setting != 3){
      speed_setting += 1;
    }
  }
  else if (dpad_value == 2 && prev_dpad != dpad_value){ // if d-pad bottom button pressed shift down in range (conditional values currently filler)
    if (speed_setting != 0){
      speed_setting -= 1;
    }
  }
  
  int prev_dpad = dpad_value; // keeps track of the dpad value to help simulate a button press
  int pulseWidthS1 = map(sensorValueS1, -512, 512, Servo_Ranges[speed_setting][0], Servo_Ranges[speed_setting][1]); // Map potentiometer value to pulse width (1000 - 2000 microSeconds)
  int pulseWidthS2 = map(sensorValueS2, -512, 512, Servo_Ranges[speed_setting][0], Servo_Ranges[speed_setting][1]); // Map potentiometer value to pulse width (1000 - 2000 microSeconds)
  myServoS1.writeMicroseconds(pulseWidthS1); // Set servo position based on pulse width
  myServoS2.writeMicroseconds(pulseWidthS2); // Set servo position based on pulse width

  int LEDValue = doc["axisRY"];
  int LED_Adjustment = map(LEDValue, -512, 512, 0, 255);
  analogWrite(LEDoutputPin, LED_Adjustment); // Set LED brightness based on the adjusted mapping from controller range to LED strip range

  Serial.print("Speed Setting: "); // print out information about sender to serial port
  Serial.print(speed_setting);
  Serial.print("Servo Pulse Width (1): ");
  Serial.print(pulseWidthS1);
  Serial.print("Servo Pulse Width (2): ");
  Serial.println(pulseWidthS2);
}

void setup() {
  Serial.begin(115200);
  WiFi.config(ip, gateway, subnet, dns); // Set the static IP before connecting
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  udp.begin(localPort);

  pinMapping();
}

void loop() {
  char incomingPacket[832];
  int packetSize = udp.parsePacket();
  
  if (packetSize) {
    int len = udp.read(incomingPacket, 832);
    if (len > 0) {
      incomingPacket[len] = 0;
    }
    
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, incomingPacket);

    if (error) {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
      return;
    }

    if doc["button_"]
  }
}