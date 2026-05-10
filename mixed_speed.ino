#include "BluetoothSerial.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SENSOR_PIN 23     // DO pin from HW-277
#define magnets 8  // change if you use more holes/marks

BluetoothSerial SerialBT;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const float Radius = 0.173;

volatile unsigned long pulseCount = 0;
volatile unsigned long lastPulseTime = 0;
volatile unsigned long pulseInterval = 0;
unsigned long lastTime = 0;
float rpm = 0;
float velocity = 0;
volatile unsigned long lastPulseMicros = 0;
const unsigned long DEBOUNCE_TIME = 6000; // 5ms in microseconds
const float maxAcceleration = 500.0;
float currentRPM = 0;
float lastRPM = 0;
float velocityKMH = 0;



void IRAM_ATTR countPulse() {
  unsigned long currentTime = micros();
  unsigned long interval = currentTime - lastPulseTime;
  if (interval > DEBOUNCE_TIME) {
    pulseCount++;
    pulseInterval = interval;
    lastPulseTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Blinky");

  lcd.init(); 
  lcd.backlight();


  pinMode(SENSOR_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), countPulse, FALLING);

  lastTime = millis();
}

void loop() {
  unsigned long currentTimes = millis();
  if (currentTimes - lastTime >= 500) {   // every .5 second
    noInterrupts();
    unsigned long pulses = pulseCount;
    Serial.print(pulseCount);
    pulseCount = 0;
    interrupts();

    rpm = (pulses * 120.0 / 2) / magnets;
    Serial.print(",| correctrpm:");
    Serial.print(rpm);
    float prpm = (pulses * 30.0) / 8.0 * (1000.0 / (currentTimes - lastTime));
    float velocity = rpm * 3 * PI * Radius / 25;
    
    Serial.print(",NEW RPM: ");
    Serial.print(prpm);
    Serial.print(", | velocity");
    Serial.print(velocity);
    SerialBT.print(" | RPM: ");
    SerialBT.println(rpm);
    lcd.clear();
    lcd.setCursor(0, 0); // Column 0, Row 0
    lcd.print("Rpm:");
    lcd.print((int)abs(rpm));
    lcd.setCursor(0, 1); 
    lcd.print("Velocity:");
    lcd.print(velocity,0);
    SerialBT.print("Velocity : ");
    SerialBT.println(velocity);
    Serial.println(velocity);

    lastTime = currentTimes;
    another();
  }
   
}
void another(){
    if (pulseInterval > 0) {
    float instantRPM = 60000000.0 / (pulseInterval * magnets);

    if (abs(instantRPM - lastRPM) < maxAcceleration || lastRPM == 0) {
      currentRPM = instantRPM;
      lastRPM = currentRPM;
    }

    velocityKMH = currentRPM * 3 * PI * Radius / 25;
  }

  if (micros() - lastPulseTime > 500000) { // 0.5 seconds of silence
    currentRPM = 0;
    velocityKMH = 0;
    lastRPM = 0;
  }


  Serial.print("RPM: "); Serial.print(currentRPM);
  Serial.print(" | Speed: "); Serial.print(velocityKMH);
  Serial.println(" km/h");
}
