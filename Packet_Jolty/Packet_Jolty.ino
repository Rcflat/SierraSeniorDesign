#include "Sabertooth.h"

Sabertooth ST(128); // The Sabertooth is on address 128. We'll name its object ST.

void setup()
{
  Serial2.begin(9600); // Start Serial2 at 9600 baud rate for Sabertooth.
  ST.autobaud();       // Send the autobaud command to the Sabertooth controller(s).
}

void loop()
{
  ST.motor(1, 127);  // Go forward at full power.
  ST.motor(2, 127);  // Go forward at full power.
  delay(2000);       // Wait 2 seconds.
  ST.motor(1, 0);    // Stop.
  ST.motor(2, 0);    // Stop.
  delay(2000);       // Wait 2 seconds.
  ST.motor(1, -127); // Reverse at full power.
  ST.motor(2, -127); // Reverse at full power.
  delay(2000);       // Wait 2 seconds.
  ST.motor(1, 0);    // Stop.
  ST.motor(2, 0);    // Stop.
  delay(2000);
}
