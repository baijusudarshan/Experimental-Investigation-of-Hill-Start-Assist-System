#include "BluetoothSerial.h"

// Define the Bluetooth object
BluetoothSerial SerialBT;

// Assuming anglex is an integer or float, declare it here
float anglex = 10.0; 

void setup() {
  Serial.begin(115200);
  // Name your Bluetooth device
  SerialBT.begin("ESP32_Blinky");
  Serial.println("Bluetooth initialized. Ready to pair!");
}

void loop() {
  // Update your angle variable here (e.g., from a sensor)
  // anglex = readSensor(); 

  // Use the correct object name: SerialBT
  SerialBT.print("angle x: ");
  SerialBT.println(anglex);
  
  // Also print to the serial monitor for debugging
  Serial.print("Sending: ");
  Serial.println(anglex);

  delay(2000);
}