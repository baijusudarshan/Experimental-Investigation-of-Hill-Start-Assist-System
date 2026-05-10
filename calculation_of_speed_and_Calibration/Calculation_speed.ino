#include "BluetoothSerial.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SENSOR_PIN 19     // DO pin from HW-277
#define PULSES_PER_REV 8  // change if you use more holes/marks
#define Rad 0.173

BluetoothSerial SerialBT;
LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile unsigned long pulseCount = 0;
volatile unsigned long lastPulseTime = 0;
unsigned long lastTime = 0;
float rpm = 0;
float velocity = 0;
volatile unsigned long lastPulseMicros = 0;
const unsigned long DEBOUNCE_TIME = 5000; // 5ms in microseconds


void IRAM_ATTR countPulse() {
  unsigned long currentMicros = micros();
  // Only count if enough time has passed since the last pulse
  if (currentMicros - lastPulseMicros > DEBOUNCE_TIME) {
    pulseCount++;
    lastPulseMicros = currentMicros;
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Blinky");

  lcd.init(); // Initialize LCD
  lcd.backlight(); // Turn on backlight


  pinMode(SENSOR_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), countPulse, FALLING);

  lastTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 500) {   // every .5 second
    noInterrupts();
    unsigned long pulses = pulseCount;
    Serial.println(pulseCount);
    pulseCount = 0;
    interrupts();

    float prpm = (pulses * 120.0 / 2) / PULSES_PER_REV;
    Serial.print("previous rpm:");
    Serial.println(prpm);
    rpm = (pulses * 30.0) / 8.0 * (1000.0 / (currentTime - lastTime));
    float velocity = rpm * 3 * PI * Rad / 25;
    Serial.print("velocity");
    Serial.println(velocity);


    Serial.print("RPM: ");
    Serial.println(rpm);
    SerialBT.print("RPM: ");
    SerialBT.println(rpm);
    lcd.clear();
    lcd.setCursor(0, 0); // Column 0, Row 0
    lcd.print("Rpm:");
    lcd.print((int)abs(rpm));
    lcd.setCursor(0, 1); 
    lcd.print("Velocity:");
    lcd.print(velocity);
    SerialBT.print("Velocity : ");
    SerialBT.println(velocity);
    // Serial.println(velocity);

    lastTime = currentTime;
  }
    // Serial.println(pulseCount);
    // Serial.print("RPM: ");
    // Serial.println(rpm);
    // SerialBT.print("RPM: ");
    // SerialBT.println(rpm);
    // lcd.clear();
    // lcd.setCursor(0, 0); // Column 0, Row 0
    // lcd.print("Speed : ");
    // lcd.print((int)abs(rpm));
}
