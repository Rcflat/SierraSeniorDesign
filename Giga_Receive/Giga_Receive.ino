#include <WiFi.h>

const char* ssid = "NETGEAR32";
const char* password = "breezybreeze113";
IPAddress receiverIP(192, 168, 1, 3); // Set the desired IP address

WiFiServer server(1883);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Set static IP address
  WiFi.config(receiverIP);
  Serial.println("Static IP configured");

  // Start server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends data
  while (!client.available()) {
    delay(1);
  }

  // Read the data and process it
  int receivedData = client.parseInt();
  processReceivedData(receivedData);

  // Close the connection
  client.stop();
}

void processReceivedData(int data) {
  Serial.print("Received: ");
  Serial.println(data);
  // Add your processing logic here
}
