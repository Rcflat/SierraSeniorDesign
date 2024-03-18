#include <WiFi.h>

const char* ssid = "NETGEAR32";
const char* password = "breezybreeze113";
const char* receiverIP = "192.168.1.3"; // Replace with the IP address of the receiving Nano ESP32

// Array to be sent
int dataArray[] = {1, 2, 3, 4, 5};

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
}

void loop() {
  // Send array over WiFi
  for (int i = 0; i < sizeof(dataArray) / sizeof(dataArray[0]); i++) {
    Serial.print("Sending: ");
    Serial.println(dataArray[i]);
    sendValueOverWiFi(dataArray[i]);
    delay(1000);
  }

  // Add a delay or any other necessary logic
}

void sendValueOverWiFi(int value) {
  WiFiClient client;
  
  if (client.connect(receiverIP, 1883)) {
    Serial.print("Sending value to ");
    Serial.println(receiverIP);
    client.print(value);
    client.stop();
  } else {
    Serial.println("Connection failed");
  }
}
