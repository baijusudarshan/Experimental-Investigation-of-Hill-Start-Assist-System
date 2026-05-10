#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
// #include <Kalman.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
Adafruit_MPU6050 mpu;
// Kalman kalmanY;
// float compAngle = 0;
unsigned long lastTime = 0;

float angleX = 0;
// unsigned long timer;
const int LED_PIN = 2; // Using the built-in LED on most ESP32s


void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  SerialBT.begin("ESP32_Blinky");
  // kalmanY.setAngle(0);
  // timer = micros();

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
  float dt = (millis() - lastTime) / 1000.0;
  lastTime = millis();

  // double dt = (double)(micros() - timer) / 1000000;
  // timer = micros();

  /* Calculate Angle (Simplified using Accelerometer)
     We use the gravity vector on the X and Z axes.
     Angle = atan2(x, z) * 180 / PI
  // */
  // float angleX = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  // double roll = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  // double gyroYrate = g.gyro.x * 180 / M_PI;
  // angleX = kalmanY.getAngle(roll, gyroYrate, dt);
  float accelAngle = atan2(a.acceleration.y, a.acceleration.z) * 180 / M_PI;
  float gyroRate = g.gyro.x * 180 / M_PI; // Convert rad/s to deg/s

  // 98% Gyro (Smooth), 2% Accel (Reference)
  angleX = 0.98 * (angleX + gyroRate * dt) + 0.02 * accelAngle;
  float cangle = - angleX ;
  Serial.print("Angle X: ");
  Serial.print(cangle,3);
  Serial.print(" raw data Angle X: ");
  Serial.println(accelAngle,2);

  // SerialBT.print("Angle X: ");
  // SerialBT.println(cangle-4,3);
  // SerialBT.print(" raw data Angle X: ");
  // SerialBT.println(accelAngle-4,2);

  // Check if angle is greater than 30 or less than -30 degrees
  if (cangle > 6) {
    digitalWrite(LED_PIN, HIGH);
    // SerialBT.println("THE ANGLE IS GREAER THEN THRESHOLD ANGLE");
  } else {
    digitalWrite(LED_PIN, LOW); // Stay off if level
    // SerialBT.println("THE ANGLE IS LESS THEN THRESHOLD ANGLE ");
  }

  delay(30);
}
