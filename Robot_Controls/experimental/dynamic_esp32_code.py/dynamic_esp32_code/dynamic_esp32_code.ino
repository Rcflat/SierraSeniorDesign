#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>  // Include the ESP32 Servo library to control servo motors

// Wi-Fi credentials
const char* ssid = "TP-Link_59D8";         
const char* password = "96332108";  

// Ports
const int discoveryPort = 5000;  // Fixed discovery port
int agreedPort = 4210;           // Default port for dynamic agreement

// UDP objects
WiFiUDP udp;
WiFiUDP communicationUdp;

// Timeout settings
const unsigned long connectionTimeout = 10000; // 10 seconds
unsigned long lastPacketTime = 0;

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

void Robot_Controls(char* incomingPacket) {

    // Serial.printf("Received packet: %s\n", incomingPacket);

    // Parse the incoming packet as JSON
    StaticJsonDocument<512> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, incomingPacket);

    if (error) {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
    } else {
      // Access JSON fields
      float axis_0 = jsonDoc["axis_0"];
      Serial.printf("axis_0: %s\n", jsonDoc["axis_0"]);
      float axis_1 = jsonDoc["axis_1"];
      float button_9 = jsonDoc["button_9"];
      float button_10 = jsonDoc["button_10"];
      int button_8 = jsonDoc["button_8"];

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

  // delay(100);  // Wait 100 ms before the next packet
}
}


void setup() {
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX2 = GPIO16, TX2 = GPIO17

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Begin listening on the discovery port
    udp.begin(discoveryPort);
    Serial.printf("Listening for discovery on port %d\n", discoveryPort);

    panServo.attach(panPin);  // Attach pan servo to pin 33
    tiltServo.attach(tiltPin);  // Attach tilt servo to pin 32
}

void loop() {
    static bool handshakeCompleted = false;

    if (!handshakeCompleted) {
        // Handle handshake process
        char incomingPacket[512];
        int packetSize = udp.parsePacket();

        if (packetSize > 0) {
            // Receive the discovery message
            int len = udp.read(incomingPacket, 512);
            if (len > 0) {
                incomingPacket[len] = '\0';
                Robot_Controls(incomingPacket);
            }

            if (strcmp(incomingPacket, "DISCOVERY") == 0) {
                // Generate a dynamic port (e.g., between 3000 and 5000)
                agreedPort = random(3000, 5000);

                // Send the proposed port back to the client
                udp.beginPacket(udp.remoteIP(), udp.remotePort());
                udp.print(agreedPort);
                udp.endPacket();
                Serial.printf("Proposed port: %d\n", agreedPort);
            } else if (strncmp(incomingPacket, "ACK:", 4) == 0) {
                // Check for acknowledgment
                int ackPort = atoi(&incomingPacket[4]);
                if (ackPort == agreedPort) {
                    Serial.printf("Acknowledgment received for port %d\n", agreedPort);
                    handshakeCompleted = true;

                    // Begin listening on the agreed port
                    communicationUdp.begin(agreedPort);
                    Serial.printf("Switched to listening on port %d\n", agreedPort);

                    // Reset the packet timer
                    lastPacketTime = millis();
                } else {
                    Serial.println("Acknowledgment mismatch. Restarting handshake.");
                }
            }
        }
    } else {
        // Handle post-handshake communication
        char incomingPacket[512];
        int packetSize = communicationUdp.parsePacket();

        if (packetSize > 0) {
            // Reset the packet timer
            lastPacketTime = millis();

            // Receive the packet
            int len = communicationUdp.read(incomingPacket, 512);
            if (len > 0) {
                incomingPacket[len] = '\0';
                Robot_Controls(incomingPacket);
            }

            Serial.printf("Received: %s\n", incomingPacket);

            // Example: Echo back the received data
            communicationUdp.beginPacket(communicationUdp.remoteIP(), communicationUdp.remotePort());
            communicationUdp.print(incomingPacket);
            communicationUdp.endPacket();
            Serial.println("Response sent.");
        } else {
            // Check if connection has timed out
            if (millis() - lastPacketTime > connectionTimeout) {
                Serial.println("Connection timed out. Returning to discovery mode.");
                handshakeCompleted = false;
                communicationUdp.stop();

                // Reinitialize the discovery UDP socket
                udp.begin(discoveryPort);
                Serial.printf("Listening for discovery on port %d\n", discoveryPort);
            }
        }
    }
}

