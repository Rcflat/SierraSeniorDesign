#include <WiFi.h>

const char *ssid = "NETGEAR32";
const char *password = "breezybreeze113";
const char *HostName = "ESP32_Slave";
IPAddress staticIP(192, 168, 1, 150); // statically set IP for ESP32_Slave
const char *receiverIP = "192.168.1.5"; // Set the IP address of the Arduino Giga R1
const uint16_t port = 4932; // Use uint16_t for port number
char  ReplyBuffer[] = "Hello";       // a string to send back


void setup() 
{
  Serial.begin(115200);//open serial via USB to PC on default port
  Serial1.begin(115200, SERIAL_8N1, 16, 17);//open the other serial port
  WiFi.setHostname(HostName);
  delay(1000);
}

void loop() {
  // **********************************************************************************************************************************************************
  Serial.println("Execution");
  while(WiFi.status() != WL_CONNECTED){
    // WifiScan(); // scan all available wifi networks that the board is within range of
  // Connect to WiFi if it isn't connected
    Serial.println("Connecting to WiFi...");
    WiFi.mode(WIFI_STA); //sets role of ESP32 to device that is connecting to access point
    Serial.println(WiFi.getHostname()); // will appear as ESP32_Slave on the router
    WiFi.config(staticIP, WiFi.gatewayIP(), WiFi.subnetMask(), WiFi.dnsIP(0), WiFi.dnsIP(1));
    WiFi.begin(ssid, password);
    delay(10000);

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Router Connection has been Established!");
      printWifiStatus();
    }
  }

  while(WiFi.status() == WL_CONNECTED) {
  // Send array over WiFi
    if (Serial1.available()) //check incoming transmission from Serial1
      { 
        sendValueOverWiFi("chicken");
      }
    else {
    }
  }
  // ***********************************************************************************************************************************************
  }

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your WiFi shield's IP address:
  Serial.print("Local IP Address: ");
  Serial.println(WiFi.localIP());
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

void sendValueOverWiFi(String value) { // used for sending packets across wifi
  WiFiClient client;
  
  if (client.connect(receiverIP, port)) {
    int bytes = client.println(value);
    Serial.println(bytes);
    client.stop();
  } else {
    Serial.println("Connection failed");
  }
}