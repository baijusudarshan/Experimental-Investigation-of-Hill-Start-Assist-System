#include "BluetoothSerial.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int sensorPin = 19; 

// constants for your 0.173m radius tyres
const float wheelRadius = 0.173; // meters
const float wheelCircumference = 2 * PI * wheelRadius;
const int magnets = 8; 

// Variables for calculation
volatile unsigned long lastPulseTime = 0;
volatile unsigned long pulseInterval = 0;
float currentRPM = 0;
float lastRPM = 0;
float velocityKMH = 0;

// Filter Settings
const unsigned long minPulseInterval = 6000; // 5ms debounce (Max ~15,000 RPM)
const float maxAcceleration = 500.0; // Max RPM change allowed per pulse

void IRAM_ATTR countPulse() {
  unsigned long currentTime = micros();
  unsigned long interval = currentTime - lastPulseTime;

  // 1. Hardware Debounce (Time Filter)
  if (interval > minPulseInterval) {
    pulseInterval = interval;
    lastPulseTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(sensorPin), countPulse, FALLING);
}

void loop() {
  // 2. Calculate RPM
  if (pulseInterval > 0) {
    // RPM = (Microseconds in a minute) / (Interval * Magnets)
    float instantRPM = 60000000.0 / (pulseInterval * magnets);

    // 3. Plausibility Check (The "ABS" Logic)
    // If RPM jump is too violent, it's likely EMI noise
    if (abs(instantRPM - lastRPM) < maxAcceleration || lastRPM == 0) {
      currentRPM = instantRPM;
      lastRPM = currentRPM;
    }

    // 4. Calculate Velocity
    // Velocity (m/s) = (RPM / 60) * Circumference
    float velocityMS = (currentRPM / 60.0) * wheelCircumference;
    velocityKMH = velocityMS * 3.6; // Convert to km/h
  }

  // 5. Timeout (If kart stops, RPM should go to 0)
  if (micros() - lastPulseTime > 500000) { // 0.5 seconds of silence
    currentRPM = 0;
    velocityKMH = 0;
    lastRPM = 0;
  }

  // Print results
  Serial.print("RPM: "); Serial.print(currentRPM);
  Serial.print(" | Speed: "); Serial.print(velocityKMH);
  Serial.println(" km/h");

  delay(100); // Small delay for Serial stability
}