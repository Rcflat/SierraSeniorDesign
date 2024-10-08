// Thermistor parameters from the datasheet
#define RT0 10000   // Resistance at 25°C for 10kΩ thermistor
#define B 3977      // Beta coefficient

// Our series resistor value = 10 kΩ
#define R 10000

// Variables for calculations for each thermistor
float RT1, VR1, ln1, TX1, VRT1;
float RT2, VR2, ln2, TX2, VRT2;
float RT3, VR3, ln3, TX3, VRT3;

// Convert T0 from Celsius to Kelvin
float T0 = 25 + 273.15;

void setup() {
  // Setup serial communication
  Serial.begin(9600);
}

void loop() {
  // ================================
  // Thermistor 1 connected to A0
  // ================================
  VRT1 = (5.00 / 1023.00) * analogRead(A0);  // Voltage across thermistor 1
  VR1 = 5.00 - VRT1;  // Voltage across the series resistor
  RT1 = VRT1 / (VR1 / R);  // Thermistor resistance
  ln1 = log(RT1 / RT0);  // Natural log of thermistor resistance ratio
  TX1 = (1 / ((ln1 / B) + (1 / T0)));  // Temperature in Kelvin
  TX1 = TX1 - 273.15;  // Convert to Celsius
  
  // Display Temperature for Thermistor 1
  Serial.print("Thermistor 1: ");
  Serial.print(TX1); Serial.print("C\t");
  Serial.print(TX1 + 273.15); Serial.print("K\t");
  Serial.print((TX1 * 1.8) + 32); Serial.println("F");
  
  // ================================
  // Thermistor 2 connected to A1
  // ================================
  VRT2 = (5.00 / 1023.00) * analogRead(A1);  // Voltage across thermistor 2
  VR2 = 5.00 - VRT2;  // Voltage across the series resistor
  RT2 = VRT2 / (VR2 / R);  // Thermistor resistance
  ln2 = log(RT2 / RT0);  // Natural log of thermistor resistance ratio
  TX2 = (1 / ((ln2 / B) + (1 / T0)));  // Temperature in Kelvin
  TX2 = TX2 - 273.15;  // Convert to Celsius
  
  // Display Temperature for Thermistor 2
  Serial.print("Thermistor 2: ");
  Serial.print(TX2); Serial.print("C\t");
  Serial.print(TX2 + 273.15); Serial.print("K\t");
  Serial.print((TX2 * 1.8) + 32); Serial.println("F");
  
  // ================================
  // Thermistor 3 connected to A2
  // ================================
  VRT3 = (5.00 / 1023.00) * analogRead(A2);  // Voltage across thermistor 3
  VR3 = 5.00 - VRT3;  // Voltage across the series resistor
  RT3 = VRT3 / (VR3 / R);  // Thermistor resistance
  ln3 = log(RT3 / RT0);  // Natural log of thermistor resistance ratio
  TX3 = (1 / ((ln3 / B) + (1 / T0)));  // Temperature in Kelvin
  TX3 = TX3 - 273.15;  // Convert to Celsius
  
  // Display Temperature for Thermistor 3
  Serial.print("Thermistor 3: ");
  Serial.print(TX3); Serial.print("C\t");
  Serial.print(TX3 + 273.15); Serial.print("K\t");
  Serial.print((TX3 * 1.8) + 32); Serial.println("F");

  // Wait 500 ms before taking the next reading
  delay(500);
}
