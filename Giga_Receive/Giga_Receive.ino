#include <WiFi.h>

const char* ssid = "NETGEAR32";
const char* password = "breezybreeze113";
const uint16_t receivePort = 4292; // Port for receiving data from ESP32_Slave
IPAddress staticIP(192, 168, 1, 149); // statically set IP for ESP32_Slave
const char *senderIP = "192.168.1.5"; // Set the IP address of the Arduino Giga R1 (not in use yet)

WiFiServer server(receivePort);

void setup(){
  Serial.begin(115220);
  delay(1000);
  Serial.println("Code Started");
}

void loop() {
  // Check for a connection to WiFi
  while (WiFi.status() != WL_CONNECTED){
  // Connect to WiFi if it isn't connected
    Serial.println("Connecting to WiFi...");
    WiFi.config(staticIP);
    WiFi.begin(ssid, password);
    delay(1000);
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Router Connection has been Established!");
      // Set static IP address for Arduino Giga R1
      printWifiStatus();
      Serial.println("Static IP configured");

      // Start server
      server.begin();
      Serial.println("Server started");
    }

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    delay(1000);
    client = server.available();
    Serial.println("Looking for Client...");
  }
  Serial.println("Client Connection Established!");

  // Wait until the client sends data
  while (!client.available()) {
    Serial.println("Waiting to Recieve...");
    delay(1000); 
  }
  // Read the data and process it
  String receivedData = client.readString();
  processReceivedData(receivedData);
  // Send data to laptop
  // Close the connection
  client.stop();
}
}

void processReceivedData(String data) {
  Serial.print("Received: ");
  Serial.println(data);
  // Add your processing logic here
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your WiFi shield's IP address:
  Serial.print("LocalIP: ");
  Serial.print(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS 1: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("DNS 2: ");
  Serial.println(WiFi.dnsIP(1));
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}