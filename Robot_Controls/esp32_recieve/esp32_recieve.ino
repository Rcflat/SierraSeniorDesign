#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

// WiFi and UDP configuration
const char *ssid = "TP-Link_59D8";   // Replace with your network's SSID
const char *password = "96332108";   // Replace with your network's password
const IPAddress localIP(192, 168, 1, 122); // ESP32's static IP
const IPAddress gateway(192, 168, 0, 1);
const IPAddress subnet(255, 255, 255, 0);
WiFiUDP udp;
unsigned int udpPort = 8888;         // Listening port for UDP
char incomingPacket[255];            // Buffer for incoming packets

// Function to send acknowledgment back to the sender
void sendAcknowledgment(IPAddress senderIP, unsigned int senderPort) {
  const char *ackMessage = "ACK";
  udp.beginPacket(senderIP, senderPort);
  udp.write((const uint8_t*)ackMessage, strlen(ackMessage));
  udp.endPacket();
  Serial.println("Acknowledgment sent.");
}

void setup() {
  Serial.begin(115200); // Start the Serial communication

  // Connect to WiFi
  WiFi.config(localIP, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start UDP
  udp.begin(udpPort);
  Serial.print("Listening on UDP port ");
  Serial.println(udpPort);
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // Packet received
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From IP: ");
    Serial.print(udp.remoteIP());
    Serial.print(", Port: ");
    Serial.println(udp.remotePort());

    // Read packet into buffer
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = '\0'; // Null-terminate the packet
      Serial.print("Packet content: ");
      Serial.println(incomingPacket);

      // Parse JSON data
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, incomingPacket);

      if (error) {
        Serial.print("Failed to parse packet: ");
        Serial.println(error.c_str());
        return;
      }

      // Example: Access axis and button data here
      float axis0 = doc["axis_0"];
      int button0 = doc["button_0"];

      Serial.println("Packet parsed successfully:");
      Serial.print("Axis 0: ");
      Serial.println(axis0);
      Serial.print("Button 0: ");
      Serial.println(button0);

      // Send acknowledgment to sender
      sendAcknowledgment(udp.remoteIP(), udp.remotePort());
    } else {
      Serial.println("Failed to read incoming packet.");
    }
  } else {
    // No packet available
    delay(10000);
    Serial.println("Nothing Happening.");
  }
}



