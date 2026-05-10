#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Kalman.h>
// #include "BluetoothSerial.h"
// #include <LiquidCrystal_I2C.h>
// #include <ArduinoOTA.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
// BluetoothSerial SerialBT;
Adafruit_MPU6050 mpu;
Kalman kalmanY;

float angleX = 0;
float angle = 0;
unsigned long timer;
const int LED_PIN = 2; // Using the built-in LED on most ESP32s

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  // SerialBT.begin("ESP32_Blinky");
  kalmanY.setAngle(0);
  timer = micros();
  lcd.init(); // Initialize LCD
  lcd.backlight(); // Turn on backlight
  Wire.begin(21,22,100000);
  Wire.setTimeOut(50);

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }

  // Set sensor ranges (Standard settings)
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  double dt = (double)(micros() - timer) / 1000000;
  timer = micros();
  double roll = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  double gyroYrate = g.gyro.x * 180 / M_PI;
  angleX = kalmanY.getAngle(roll, gyroYrate, dt);

  float accelAngle = atan2(a.acceleration.y, a.acceleration.z) * 180 / M_PI;
  float gyroRate = g.gyro.x * 180 / M_PI; // Convert rad/s to deg/s

  // 98% Gyro (Smooth), 2% Accel (Reference)
  angle = 0.98 * (angle + gyroRate * dt) + 0.02 * accelAngle;
  float cangle = - angle ;

  // Serial.print("complementary filter : ");
  // Serial.println(cangle);
  // SerialBT.print("Angle X: ");
  // SerialBT.println(angleX);
  // lcd.clear();
  // lcd.setCursor(0,0);
  // lcd.print((int)abs(angleX));
  Serial.print("RAW");
  Serial.print(roll);
  Serial.print(",");
  Serial.print("||Kalman angle: ");
  Serial.println(-angleX);
  if (abs(cangle) > 6) {
    digitalWrite(LED_PIN, HIGH);
    // SerialBT.println("THE ANGLE IS GREAER THEN THRESHOLD ANGLE SO LIGHT IS ON");
  } else {
    digitalWrite(LED_PIN, LOW); // Stay off if level
    // SerialBT.println("THE ANGLE IS LESS THEN THRESHOLD ANGLE HENCE THE LIGHT IS OFF");
  }

  delay(30);
}
