 // Pin Definitions
const int thermistorPin1 = 11; // GPIO 32 for thermistor 1
const int thermistorPin2 = 10; // GPIO 33 for thermistor 2
const int thermistorPin3 = 9; // GPIO 34 for thermistor 3

// Constants for calculating temperature
const float referenceResistance = 100000; // 10k ohm reference resistor
const float nominalResistance = 10000;   // 10k ohm thermistor resistance at 25°C
const float nominalTemperature = 25;     // Nominal temperature (25°C)
const float betaCoefficient = 3950;      // Beta coefficient for the thermistor
const float seriesResistor = 100000;      // Value of the series resistor

float readTemperature(int pin) {
  // Read the analog value (ADC)
  int analogValue = analogRead(pin);
  
  // Convert the analog value to resistance
  float voltage = analogValue * 3.3 / 4095.0;
  float resistance = (3.3 / voltage - 1) * seriesResistor;

  // Calculate temperature in Kelvin using the Beta equation
  float temperatureKelvin = 1 / (log(resistance / nominalResistance) / betaCoefficient + 1 / (nominalTemperature + 273.15));
  
  // Convert Kelvin to Celsius
  float temperatureCelsius = temperatureKelvin - 273.15;
  
  return temperatureCelsius;
}

void setup() {
  Serial.begin(115200);

  // Initialize the analog pins
  pinMode(thermistorPin1, INPUT);
  pinMode(thermistorPin2, INPUT);
  pinMode(thermistorPin3, INPUT);
}

void loop() {
  // Read temperatures from all three thermistors
  float temperature1 = readTemperature(thermistorPin1);
  float temperature2 = readTemperature(thermistorPin2);
  float temperature3 = readTemperature(thermistorPin3);

  // Print the results
  Serial.print("Temperature 1: ");
  Serial.print(temperature1);
  Serial.println(" °C");

  Serial.print("Temperature 2: ");
  Serial.print(temperature2);
  Serial.println(" °C");

  Serial.print("Temperature 3: ");
  Serial.print(temperature3);
  Serial.println(" °C");

  // Wait 1 second before taking new readings
  delay(1000);
} 