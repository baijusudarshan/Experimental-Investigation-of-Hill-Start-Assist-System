#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Kalman.h>
#include "BluetoothSerial.h"
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

// PINS DEFINATION
const int LOADCELL_DOUT_PIN = 27; // load cell input pin
const int LOADCELL_SCK_PIN = 26;   // load cell define pin
const int SPEED_PIN = 23;         // for speed sensor  
const int MAGNET = 8;             // total no of magnet
const int CLUTCH_PIN = 19;        // clutch input signal pin
const int THROTTEL_PIN = 18;      // Throttel input signal pin
const int RELAY_PIN = 32;          // This pin used for controlling high voltage from MOSFET like Relay
const int LED_PIN = 2;

LiquidCrystal_I2C lcd(0x27, 16, 2); 
BluetoothSerial SerialBT;
Adafruit_MPU6050 mpu;
Kalman kalmanY;
HX711 scale;

// mpu defination
float angleX = 0; 
unsigned long lastGyroTime = 0;
unsigned long lastmaxtime = 0;
int angle;
float appliedforce;

// speed defination or requirement
float rpm = 0;
float velocity = 0;
const float Radius = 0.173; // Radius of Go-kart
volatile unsigned long pulseCount = 0;
volatile unsigned long lastPulseTime = 0;
const unsigned long DEBOUNCE_TIME = 6000; // 6ms in microseconds minimum time for two point of magnet
unsigned long lastTime = 0;

// TABLE DATA
int ForceTable[] = {
  12,  // angle 6
  15,  // angle 7
  17,  // angle 8
  19,  // angle 9
  21,  // angle 10
  22, // angle 11
  24, // angle 12
  26, // angle 13
  28, // angle 14
  30, // angle 15
  32  // angle 16
};

int getRelativeForce(int ang) {
  if (ang < 6) return 0;
  if (ang > 16) ang = 16;
  return ForceTable[ang - 6];
}

void IRAM_ATTR countPulse() {
  unsigned long currentTime = micros();
  unsigned long interval = currentTime - lastPulseTime;
  if (interval > DEBOUNCE_TIME) { // Only count if enough time has passed since the last pulse
    pulseCount++;
    lastPulseTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Blinky");
  Wire.begin(21,22,100000);
  Wire.setTimeOut(50);
  kalmanY.setAngle(0);
  lcd.init(); // Initialize LCD
  lcd.backlight(); // Turn on backlight
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  unsigned long timeout = millis();
  while (!scale.is_ready() && millis() - timeout < 2000) {
    delay(10);
  }
  
  if (scale.is_ready()) {
    scale.set_scale(58000.f); // Set your calibration factor here once!
    scale.tare();
    Serial.println("HX711 Ready");
  } else {
    Serial.println("HX711 NOT FOUND during setup - check wiring!");
  }

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SPEED_PIN, INPUT_PULLUP);
  pinMode(CLUTCH_PIN, INPUT_PULLUP);
  pinMode(THROTTEL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SPEED_PIN), countPulse, FALLING);

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }
  
  // Set sensor ranges (Standard settings)
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  lastTime = millis();
  lastGyroTime = millis();
  lastmaxtime = millis();
}

void loop() {
  // main function
  speed();
  brake_force();
  slope();
  maxtimer();
  if (appliedforce > 8){
    rpm = 0;
  }
  
  Serial.print("  || RPM: ");
  Serial.print(rpm);
  Serial.print("  || Velocity : ");
  Serial.print(velocity, 0);
  Serial.print("Km/hr");
  Serial.print("  ||  ANGLE : ");
  Serial.print(angle); 
  Serial.print("  || FORCE : ");
  Serial.print(appliedforce, 0);
  Serial.print("N");
  Serial.print(" || Throttel :");
  Serial.print(digitalRead(THROTTEL_PIN) == HIGH ? "0" : "1");
  Serial.print(" || clutch :");
  Serial.print(digitalRead(CLUTCH_PIN) == HIGH ? "0" : "1");
  Serial.print(" ||HSA :");
  Serial.println(digitalRead(LED_PIN) == HIGH ? "ON" : "OFF");

  int ThresholdForce = getRelativeForce(angle);
  int clutchsensorState = digitalRead(CLUTCH_PIN);
  int throttelsensorState = digitalRead(THROTTEL_PIN);

  // main logic here
  if (velocity > 1 || angle < 5) {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }
  else if (throttelsensorState == LOW) {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }
  else if (angle >=  && appliedforce > ThresholdForce && rpm < 10 && clutchsensorState == LOW) {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
  }

  delay(30);
}

void speed() {
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 500) {   // every .5 second calculation of speed
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;
    interrupts();

    rpm = (pulses * 120.0 / 2) / MAGNET;
    velocity = rpm * 3 * PI * Radius / 25; 
    lcd.clear();
    lcd.setCursor(0, 0); // Column 0, Row 0
    lcd.print("Rpm:");
    lcd.print(rpm, 0);
    lcd.print(" Ang:");
    lcd.print(angle,0);
    lcd.setCursor(0, 1); 
    lcd.print("Speed:");
    lcd.print(velocity, 0);
    lcd.print(" HSA:");
    lcd.print(digitalRead(LED_PIN) == HIGH ? "ON" : "OFF");
    lastTime = currentTime;
  } 
}

void brake_force() {
  if (scale.is_ready()) {
    // scale.set_scale();
    long reading = scale.get_units(5);
    appliedforce = reading;
  }
}

void slope() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  unsigned long timer = micros();
  double dt = (timer - lastGyroTime) / 1000000.0;
  lastGyroTime = timer;
  
  double roll = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  double gyroYrate = g.gyro.x * 180 / M_PI;
  angleX = kalmanY.getAngle(roll, gyroYrate, dt);
  // angle =angleX;
  float accelAngle = atan2(a.acceleration.y, a.acceleration.z) * 180 / M_PI;
  float gyroRate = g.gyro.x * 180 / M_PI; // Convert rad/s to deg/s
  angle = 0.98 * (angle + gyroRate * dt) + 0.02 * accelAngle;
}
void maxtimer(){
  if(digitalRead(RELAY_PIN) == HIGH){
    unsigned long currentTimes = millis()
    if (currentTimes - lastmaxtime >= 30000){
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(LED_PIN, LOW);
    }
  }

}
