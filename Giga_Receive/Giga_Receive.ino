#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <Servo.h>

JsonDocument doc;

int prev_dpad; 
int status = WL_IDLE_STATUS;
char ssid[] = "NETGEAR32"; //  your network SSID (name)
char pass[] = "breezybreeze113";    // your network password (use for WPA, or use as key for WEP)
unsigned int localPort = 4292; // local port to listen on
char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged"; // awknowledgement string

WiFiUDP Udp;

Servo myServoS1;
Servo myServoS2;

void setup() {
  
  Serial.begin(115200); //Initialize serial and wait for port to open:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  initializationWiFiStatus(); // initialize the WiFi on boot (plus firmwave check)
  pinMapping();

}

void loop() {
  int packetSize = Udp.parsePacket(); // if there's data being recieved from a client, read the packet
  if (packetSize) {
    Serial.print("Received packet of size "); // print out information about sender to serial port
    Serial.print(packetSize);
    Serial.print(" From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    int len = Udp.read(packetBuffer, 255); // print contents of packet to serial port for monitoring
    if (len > 0) {
      packetBuffer[len] = 0;
      // Serial.println("Contents:");
      // Serial.println(packetBuffer);
      deserializeJson(doc, packetBuffer);
      updateValues();
    }

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); // send a reply, to the IP address and port that sent us the packet we received
    Udp.write(ReplyBuffer);
    Udp.endPacket();

    
  }
}

void initializationWiFiStatus(){ // used in setup put here to improve readability
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
    status = WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    delay(1000); // wait 1 second for connection:
    if (status == WL_CONNECTED){
      Serial.println("Connected to wifi");
      printWifiStatus();
      Serial.println("\nStarting connection to server...");
      Udp.begin(localPort); // connect to specified port on the router once connection is established
    }
  }
}

void pinMapping(){ // for now only works with controller
  const int outputPinS1 = 2; // PWM output pin for S1
  const int outputPinS2 = 3; // PWM output pin for S2
  const int LEDoutputPin = 9; // output pin for LED

  myServoS1.attach(outputPinS1); // Attaching servo to the pin
  myServoS2.attach(outputPinS2); // Attaching servo to the pin
} 

void updateValues(){ // for now only works with controller

  int sensorValueS1 = doc["axisY"];  //analogRead(potPin); // Read potentiometer value (-512-512)
  int sensorValueS2 = doc["axisY"];  //analogRead(potPin); // Read potentiometer value (-512-512)

  // d-pad logic will go on client side in the future but for the preview day I am putting it on the robot to make my life simpler
  int Servo_Ranges[][] = {{1000, 1050},{1000, 1150},{1000, 1500},{1000, 2000}}; // desired speed ranges per speed setting
  int speed_setting = 0; // value to index the Servo_Ranges 2-dimensional array
  int dpad_value = doc["dpad"];
  if (dpad_value == 1 && prev_dpad != 1){ // if d-pad top button pressed shift up in range (conditional values currently filler)
    if (speed_setting != 3){
      speed_setting += 1;
    }
  }
  else if (dpad_value == 4 && prev_dpad != 4){ // if d-pad bottom button pressed shift down in range (conditional values currently filler)
    if (speed_setting != 0){
      speed_setting -= 1;
    }
  }
  int prev_dpad = dpad_value; // keeps track of the dpad value to help simulate a button press
  int pulseWidthS1 = map(sensorValueS1, -512, 512, Servo_Ranges[speed_setting][0], Servo_Ranges[speed_setting][1]); // Map potentiometer value to pulse width (1000 - 2000 microSeconds)
  int pulseWidthS2 = map(sensorValueS2, -512, 512, Servo_Ranges[speed_setting][0], Servo_Ranges[speed_setting][1]); // Map potentiometer value to pulse width (1000 - 2000 microSeconds)
  myServoS1.writeMicroseconds(pulseWidthS1); // Set servo position based on pulse width
  myServoS2.writeMicroseconds(pulseWidthS2); // Set servo position based on pulse width

  int LEDValue = doc["axisRY"];
  int LED_Adjustment = map(LEDValue, -512, 512, 0, 255);
  analogWrite(LEDoutputPin, LED_Adjustment); // Set LED brightness based on the adjusted mapping from controller range to LED strip range
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}