#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>  // Include the ESP32 Servo library to control servo motors

// Wi-Fi configuration
const char* ssid = "TP-Link_59D8";         
const char* password = "96332108";  

// UDP configuration
WiFiUDP udp;
unsigned int localUdpPort = 4210;

int motor1Speed = 0;
int motor2Speed = 0;
char motor1SpeedChar[20];
char motor2SpeedChar[20];

// Define servo objects for pan and tilt movements
Servo panServo;
Servo tiltServo;

// Define pin numbers for the pan and tilt servos
int panPin = 33;
int tiltPin = 32;

float pan_angle = 100;
float tilt_angle = 120;

void SendSabertoothCommand(int motor1Speed, int motor2Speed) {

  sprintf(motor1SpeedChar, "M1: %d \r\n", motor1Speed);
  Serial2.print(motor1SpeedChar);
  sprintf(motor2SpeedChar, "M2: %d \r\n", motor2Speed);
  Serial2.print(motor2SpeedChar);
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX2 = GPIO16, TX2 = GPIO17
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  udp.begin(localUdpPort);
  Serial.printf("Listening on IP: %s, Port: %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  panServo.attach(panPin);  // Attach pan servo to pin 33
  tiltServo.attach(tiltPin);  // Attach tilt servo to pin 32

  
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incomingPacket[415];
    int len = udp.read(incomingPacket, 415);
    if (len > 0) {
      incomingPacket[len] = '\0';
    }
    Serial.printf("Received: %d\n", incomingPacket);

    // Parse the incoming packet as JSON
    StaticJsonDocument<415> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, incomingPacket);

    if (error) {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
    } else {
      // Access JSON fields
      float axis_0 = jsonDoc["axis_0"];
      float axis_1 = jsonDoc["axis_1"];
      float button_9 = jsonDoc["button_9"];
      float button_10 = jsonDoc["button_10"];
      int button_8 = jsonDoc["button_8"];

      // Serial.printf("axis_0: %f\n", axis_0);
      // Serial.printf("axis_1: %f\n", axis_1);


      // Differential drive calculations (using left-stick)
      float left_motor = -axis_0 - axis_1;   // Adjusted for Sabertooth's control range
      float right_motor = -axis_1 + axis_0;

      // Map values to Sabertooth motor range (idk to idk)
      int leftMotorValue = (int)(left_motor * 1500);
      int rightMotorValue = (int)(right_motor * 1500);

      // Rear bumper logic
      if (button_9 == 1){
        leftMotorValue = -3000;
        rightMotorValue = -3000;
      }

      if (button_10 == 1){
        leftMotorValue = 3000;
        rightMotorValue = 3000;
      }

      if (leftMotorValue <= 200  && leftMotorValue >= -200){
        leftMotorValue = 0;
      }

      if (rightMotorValue <= 200 && rightMotorValue >= -200){
        rightMotorValue = 0;
      }

      // Debug print for motor values
      // Serial.printf("leftMotorValue: %d\n", leftMotorValue);
      // Serial.printf("rightMotorValue: %d\n", rightMotorValue);

      // Set motor speed and direction using Sabertooth
      SendSabertoothCommand(leftMotorValue, rightMotorValue);

      // Set angle of pan-tilt
      float axis_2 = jsonDoc["axis_2"];
      float axis_3 = jsonDoc["axis_3"];

      // Servo Value are 0 - 180
      pan_angle = pan_angle + (axis_2 * 0.2);
      tilt_angle = tilt_angle + (axis_3 * 0.2);

      if (pan_angle >= 160){
        pan_angle = 160;
      }

      if (tilt_angle >= 130){
        tilt_angle = 130;
      }

      if (pan_angle <= 20){
        pan_angle = 20;
      }

      if (tilt_angle <= 50){
        tilt_angle = 50;
      }

      if (button_8 == 1){
        pan_angle = 100;
        tilt_angle = 120;
      }

      // Debug print for motor values
      Serial.printf("pan-angle: %d\n", pan_angle);
      Serial.printf("tilt-angle: %d\n", tilt_angle);

      // Set both pan and tilt servos to the specified angle
      panServo.write(pan_angle);
      tiltServo.write(tilt_angle);

      // Send acknowledgment message
      const char* response = "ESP32: JSON message received";
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.write((const uint8_t*)response, strlen(response));
      udp.endPacket();
    }
  }

  // Send a periodic message to the computer
  const char* periodicMessage = "ESP32: Keep alive";
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.write((const uint8_t*)periodicMessage, strlen(periodicMessage));
  udp.endPacket();

  // delay(100);  // Wait 100 ms before the next packet
}
