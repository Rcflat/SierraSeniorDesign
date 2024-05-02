/*
  WiFi UDP Send and Receive String
  This sketch wait an UDP packet on localPort using a WiFi shield.
  When a packet is received an Acknowledge packet is sent to the client on port remotePort
  Circuit:
  * WiFi shield attached
  created 30 December 2012
  by dlf (Metodo2 srl)
*/

#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>

JsonDocument doc;

int status = WL_IDLE_STATUS;
char ssid[] = "NETGEAR32"; //  your network SSID (name)
char pass[] = "breezybreeze113";    // your network password (use for WPA, or use as key for WEP)

unsigned int localPort = 4292;      // local port to listen on

char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";       // a string to send back

WiFiUDP Udp;

void setup() {
  
  Serial.begin(115200); //Initialize serial and wait for port to open:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  if (WiFi.status() == WL_NO_SHIELD) { // check for the presence of the shield:
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion(); // check firmware version
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }
  
  while (status != WL_CONNECTED) { // attempt to connect to Wifi network:
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(1000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
}

void loop() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.print(packetSize);
    Serial.print(" From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());
    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
      Serial.println("Contents:");
      Serial.println(packetBuffer);
    // serializeJson(doc, Serial);
    }
  


    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}