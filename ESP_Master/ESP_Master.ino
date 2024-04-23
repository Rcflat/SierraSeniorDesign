void setup() 
{
  Serial.begin(115200);//open serial via USB to PC on default port
  Serial1.begin(115200, SERIAL_8N1, 16, 17);//open the other serial port
  delay(1000);
}

void loop() 
{  
  size_t bytes = Serial1.println("hello");
  if (bytes) {
    Serial.println("Transmitting Information");
    delay(1000);
  }
  else {
    Serial.print("Waiting...");
  }
  delay(1000);
}