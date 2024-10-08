const int numReadings = 30;
float readings[numReadings];      // the readings from the analog input
int readingIndex = 0;             // the index of the current reading
float total = 0;                  // the running total
float average = 0;                // the average

float currentValue = 0;           // Smoothed current value
const int sensorPin = 25;         // GPIO pin for the current sensor

// Battery-specific parameters
const float batteryCapacity = 2200.0;  // Battery capacity in mAh (example: 2200 mAh)
float stateOfCharge = 100.0;           // Initial state of charge percentage
unsigned long previousTime = 0;

void setup() {
  Serial.begin(115200);
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;

  // Set up ADC resolution if needed (default is 12-bit)
  // analogReadResolution(10);  // Uncomment if you want 10-bit resolution like Arduino
}

void loop() {
    total = total - readings[readingIndex];

    // Read raw sensor data
    readings[readingIndex] = analogRead(sensorPin); 

    // Convert raw data to current (adjust the formula based on your sensor's sensitivity)
    readings[readingIndex] = (readings[readingIndex] - 2048) * 5.0 / 4096 / 0.04 - 6.30;

    total = total + readings[readingIndex];
    readingIndex = readingIndex + 1;

    if (readingIndex >= numReadings)
      readingIndex = 0;

    // Apply smoothing algorithm
    average = total / numReadings;
    currentValue = average;  // Current in amperes

    // Calculate time delta in seconds
    unsigned long currentTime = millis();
    float timeElapsed = (currentTime - previousTime) / 1000.0; // Time in seconds
    previousTime = currentTime;

    // Update state of charge using coulomb counting
    // batteryCapacity is in mAh, convert current to mA and integrate over time
    stateOfCharge -= (currentValue * 1000.0 * timeElapsed / 3600.0) / batteryCapacity;

    // Constrain SOC to valid range (0% to 100%)
    stateOfCharge = constrain(stateOfCharge, 0, 100);

    // Output the current value and state of charge
    Serial.print("Current (A): ");
    Serial.print(currentValue);
    Serial.print("\tState of Charge (%): ");
    Serial.println(stateOfCharge);

    delay(100);  // Adjust as needed to control how often readings are taken
}
