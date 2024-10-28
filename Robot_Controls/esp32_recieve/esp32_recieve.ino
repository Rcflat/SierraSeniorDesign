#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>  // Include the ESP32Servo library

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

// PINS for Thermister 25, 26, 27

// WiFi credentials
const char* ssid = "NETGEAR32";
const char* password = "breezybreeze113";

// Static IP address configuration
IPAddress ip(192, 168, 1, 50);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);

WiFiUDP udp;
const unsigned int localPort = 8888;

// Create Servo objects
Servo myServoS1;
Servo myServoS2;

void pinMapping() {
  const int outputPinS1 = 2;  // PWM output pin for S1
  const int outputPinS2 = 3;  // PWM output pin for S2

  myServoS1.attach(outputPinS1); // Attach servo to the pin
  myServoS2.attach(outputPinS2); // Attach servo to the pin
}

void updateValues(JsonDocument& doc) {
  int sensorValueS1 = doc["axisX"];  
  int sensorValueS2 = doc["axisY"];  

  int Servo_Ranges[4][2] = {{1450, 1500}, {1400, 1550}, {1200, 1750}, {1000, 1950}};
  int dpad_value = doc["dpad"];
  static int prev_dpad = 0;
  static int speed_setting = 0;

  if (dpad_value == 1 && prev_dpad != dpad_value) {
    if (speed_setting < 3) {
      speed_setting++;
    }
  } else if (dpad_value == 2 && prev_dpad != dpad_value) {
    if (speed_setting > 0) {
      speed_setting--;
    }
  }

  prev_dpad = dpad_value;

  // Map sensor values to servo ranges (in degrees)
  int angleS1 = map(sensorValueS1, -512, 512, 0, 180);
  int angleS2 = map(sensorValueS2, -512, 512, 0, 180);
  
  myServoS1.write(angleS1); // Set the servo to the specified angle
  myServoS2.write(angleS2); // Set the servo to the specified angle

  int LEDValue = doc["axisRY"];
  int LED_Adjustment = map(LEDValue, -512, 512, 0, 255);
  analogWrite(9, LED_Adjustment); 

  Serial.print("Speed Setting: ");
  Serial.print(speed_setting);
  Serial.print(" Servo Angle (1): ");
  Serial.print(angleS1);
  Serial.print(" Servo Angle (2): ");
  Serial.println(angleS2);
}

void setup() {
  Serial.begin(115200);
  WiFi.config(ip, gateway, subnet, dns);
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

    updateValues(doc);
  }

  // If no packet, robot defaults to controller rest values
}
