#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Kalman.h>
#include <LiquidCrystal_I2C.h>

// --- Device Setup ---
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_MPU6050 mpu;
Kalman kalmanY;

// --- Filter Variables ---
float compAngle = 0;       
float kalmanAngle = 0;     
unsigned long timer;
bool initialized = false;  
const int LED_PIN = 2;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }

  // Optimize ranges for kart dynamics
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  lcd.init();
  lcd.backlight();
  timer = micros();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // 1. Calculate precise delta time
  double dt = (double)(micros() - timer) / 1000000;
  timer = micros();

  // 2. Extract Raw Inputs
  float roll = atan2(a.acceleration.y, a.acceleration.z) * 180 / M_PI;
  float gyroRate = g.gyro.x * 180 / M_PI;

  // 3. INITIALIZATION: Sync filter with physical angle on start
  if (!initialized) {
    compAngle = roll;
    kalmanY.setAngle(roll);
    initialized = true;
  }

  // 4. Calculate Filters
  kalmanAngle = kalmanY.getAngle(roll, gyroRate, dt);
  
  // Complementary Formula: Trust 98% Gyro + 2% Accelerometer
  compAngle = 0.98 * (compAngle + (gyroRate * dt)) + 0.02 * roll;

  // 5. Output to Serial (Format: RAW, Angle X, Filter)
  Serial.print("RAW"); Serial.print(gyroRate, 2);
  Serial.print(", Angle X: "); Serial.print(kalmanAngle, 2);
  Serial.print(", complementary filter: "); Serial.println(compAngle, 2);

  // 6. Visual Feedback (LCD & LED)
  lcd.setCursor(0,0);
  lcd.print("Tilt: "); lcd.print(compAngle, 1);
  lcd.print(" deg  "); 

  // LED Threshold for Hill Assist engagement
  if (abs(compAngle) > 6) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  delay(30); 
}