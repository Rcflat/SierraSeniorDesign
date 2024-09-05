#include <KeyboardController.h>
#include <MouseController.h>
#include <Usb.h>
#include <hidboot.h>
#include <hidusagestr.h>
#include <adk.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <Servo.h>

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

Servo myServoS1;
Servo myServoS2;

USB usb;  // USB Host object
ADK adk(&usb); // ADK object with USB host passed

void pinMapping() {
  const int outputPinS1 = 2;  // PWM output pin for S1
  const int outputPinS2 = 3;  // PWM output pin for S2
  const int outputPinS3 = 4;  // Servo output pin for S3
  const int outputPinS4 = 5;  // Servo output pin for S4
  const int LEDoutputPin = 9; // Output pin for LED

  myServoS1.attach(outputPinS1); // Attaching servo to the pin
  myServoS2.attach(outputPinS2); // Attaching servo to the pin
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

  int pulseWidthS1 = map(sensorValueS1, -512, 512, Servo_Ranges[speed_setting][0], Servo_Ranges[speed_setting][1]);
  int pulseWidthS2 = map(sensorValueS2, -512, 512, Servo_Ranges[speed_setting][0], Servo_Ranges[speed_setting][1]);
  
  myServoS1.writeMicroseconds(pulseWidthS1);
  myServoS2.writeMicroseconds(pulseWidthS2);

  int LEDValue = doc["axisRY"];
  int LED_Adjustment = map(LEDValue, -512, 512, 0, 255);
  analogWrite(9, LED_Adjustment); 

  Serial.print("Speed Setting: ");
  Serial.print(speed_setting);
  Serial.print(" Servo Pulse Width (1): ");
  Serial.print(pulseWidthS1);
  Serial.print(" Servo Pulse Width (2): ");
  Serial.println(pulseWidthS2);
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

  usb.Init();  // Initialize USB Host
  adk.begin("Arduino", "ADK", "My Arduino ADK", "0.1", "http://arduino.cc", "0000000012345678");

  pinMapping();
}

void loop() {
  usb.Task();  // Polling the USB bus
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

    // Trigger ADB command to wake up phone using ADK
    const char wakeup_command[] = "adb shell input keyevent KEYCODE_WAKEUP";
    adk.write((uint8_t*)wakeup_command, sizeof(wakeup_command));
  }
}
