#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Kalman.h>
#include "BluetoothSerial.h"
#include <LiquidCrystal_I2C.h>
#include <ArduinoOTA.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
BluetoothSerial SerialBT;
Adafruit_MPU6050 mpu;
Kalman kalmanY;

float angleX = 0;
static float angle = 0;
unsigned long timer;
const int LED_PIN = 2; // Using the built-in LED on most ESP32s
float velocity_y = 0;
float finalvelocity_y = 0;
float accel_error_y = 0;
const int error_samples = 200;
unsigned long last_time = 0;
const float high_pass_alpha = 0.95;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(LED_PIN, OUTPUT);
  SerialBT.begin("ESP32_Blinky");
  kalmanY.setAngle(0);
  timer = micros();
  lcd.init(); // Initialize LCD
  lcd.backlight(); // Turn on backlight

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }

  // Set sensor ranges (Standard settings)
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  calibrateSensor();
  // last_time = micros();
}

void calibrateSensor() {
  Serial.println("Calibrating Y-axis...");
  float sum_y = 0;
  
  for (int i = 0; i < error_samples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    sum_y += a.acceleration.y;
    delay(5);
  }
  accel_error_y = sum_y / error_samples;
  Serial.println("Calibration complete!");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  double dt = (double)(micros() - timer) / 1000000;
  timer = micros();

  double roll = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  double gyroYrate = g.gyro.x * 180 / M_PI;
  angleX = -(kalmanY.getAngle(roll, gyroYrate, dt));

  
  float accelAngle = atan2(a.acceleration.y, a.acceleration.z) * 180 / M_PI;
  float gyroRate = g.gyro.x * 180 / M_PI; // Convert rad/s to deg/s

  // 98% Gyro (Smooth), 2% Accel (Reference)
  angle = ( 0.98 * (angle + gyroRate * dt) + 0.02 * accelAngle );
  float cangle = - angle ;

  // Serial.print("complementary filter : ");
  // Serial.println(cangle);
  SerialBT.print("Angle X: ");
  SerialBT.println(angleX);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print((int)abs(angleX));
  Serial.print("RAW");
  Serial.print(roll,0);
  // Serial.print(",");
  Serial.print(",Angle X: ");
  Serial.print(angleX,0);
  Serial.print(",complementary filter : ");
  Serial.print(cangle,0);

  // Check if angle is greater than 30 or less than -30 degrees
 
  // unsigned long current_time = micros();
  // dta = (current_time - last_time) / 1000000.0;
  // last_time = current_time;


  float raw_accel_y = a.acceleration.y - accel_error_y;
  static float last_raw_y = 0;
  static float filtered_accel_y = 0;
  filtered_accel_y = high_pass_alpha * (filtered_accel_y + raw_accel_y - last_raw_y);
  last_raw_y = raw_accel_y;
  if (abs(filtered_accel_y) < 0.05) filtered_accel_y = 0;

  velocity_y += filtered_accel_y * dt;

  if (abs(filtered_accel_y) < 0.02) {
    velocity_y *= 0.96; 
  }
  finalvelocity_y = velocity_y * 3.6 ;
  Serial.print(",acceleratation "); 
  Serial.print(filtered_accel_y, 4);
  Serial.print(" Vel_Y:"); 
  Serial.print(abs(velocity_y), 4);
  Serial.print("M/s and "); 
  Serial.print(abs(finalvelocity_y), 4);
  Serial.println("Km/hr");
  SerialBT.print(abs(finalvelocity_y), 0);
  SerialBT.print("Km/hr");  


  if ((cangle > 6) && (abs(velocity_y) < 0.5) ) {
    digitalWrite(LED_PIN, HIGH);
    // SerialBT.println("THE ANGLE IS GREAER THEN THRESHOLD ANGLE SO LIGHT IS ON");
  } else {
    digitalWrite(LED_PIN, LOW); // Stay off if level
    // SerialBT.println("THE ANGLE IS LESS THEN THRESHOLD ANGLE HENCE THE LIGHT IS OFF");
  }

  delay(10);
}
