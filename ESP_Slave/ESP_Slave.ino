#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>

char ssid[] = "NETGEAR32";          // your network SSID (name)
char pass[] = "breezybreeze113";   // your network password

unsigned int localPort = 4290;      // Local port to listen on

char serverIP[] = "192.168.1.4";    // IP address of the Arduino server
unsigned int serverPort = 4292;     // Port of the Arduino server

WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 16, 17);
  while (!Serial) {
    ; // Wait for Serial to be ready
  }

  Serial.println("Connecting to WiFi...");
  connectToWiFi();  // Connect to WiFi network
  Serial.println("WiFi connected");
}

void loop() {
  // Handle Serial1 data if available
  int packetBuffer = Serial1.read();
  if (packetBuffer) {
    sendMessage(packetBuffer);
  }
  // Handle UDP packet if available
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char packetBuffer[255];
    udp.read(packetBuffer, 255);
    Serial.print("Received from server: ");
    Serial.println(packetBuffer);
  }

  // Other non-blocking tasks can be added here

  delay(150); // This delay is optional and can be adjusted based on your requirements
}

void connectToWiFi() {
  WiFi.begin(ssid, pass);  // Connect to WiFi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.println("Retrying WiFi connection...");
  }
}

void sendMessage(int packetBuffer) {
  udp.beginPacket(serverIP, serverPort);  // Begin sending data to server
  udp.print(packetBuffer);
  udp.endPacket();  // End sending data
}
