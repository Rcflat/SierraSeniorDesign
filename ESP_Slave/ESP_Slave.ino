#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

JsonDocument doc;

char ssid[] = "NETGEAR32";          // your network SSID (name)
char pass[] = "breezybreeze113";   // your network password

unsigned int localPort = 4290;      // Local port to listen on

char serverIP[] = "192.168.1.4";    // IP address of the Arduino server
unsigned int serverPort = 4292;     // Port of the Arduino server
char packetBuffer2[255];

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
  
  if (Serial1.available()) { // wait until there is information recived from the Serial1 port
    String packetBuffer1 = Serial1.readStringUntil('\n'); // parse the information through the use of newline characters
    udp.beginPacket(serverIP, serverPort);  // Begin sending data to server
    udp.write((uint8_t*)packetBuffer1.c_str(), 255);
    udp.endPacket();  // End sending data
    Serial.println(packetBuffer1);
  }
  
  int packetSize = udp.parsePacket(); // await awknowledgement from the server that the JSON object was recieved
  if (packetSize) {
    udp.read(packetBuffer2, 255);
    Serial.print("Received from server: ");
    Serial.println(packetBuffer2);
  }
}

void connectToWiFi() {
  WiFi.begin(ssid, pass);  // Connect to WiFi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.println("Retrying WiFi connection...");
  }
}

