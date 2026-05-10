#include "HX711.h"
#include "BluetoothSerial.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define known_weight 500.0

// Pin definitions
const int LOADCELL_DOUT_PIN = 27;
const int LOADCELL_SCK_PIN = 26;
const int led = 2;


HX711 scale;
BluetoothSerial SerialBT;


void calibration(){   
    Serial.println("Tare... remove all weights");
    delay(5000);
    scale.tare();        // Reset the scale to 0
    Serial.println("Tare done...");
    Serial.print("Place known weight and check readings...");
    delay(5000);

}

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  SerialBT.begin("ESP32_Blinky");
  Serial.println("Bluetooth initialized. Ready to pair!");

  calibration();

}

void loop() {
  if (scale.is_ready()) {
    scale.set_scale(); 
    long reading = scale.get_units(10);
    Serial.print("Result: ");
    Serial.println(reading);
    SerialBT.print("Result: ");
    SerialBT.println(reading);
    float C_factor = reading / known_weight ;
    float weight = reading / (58*1000);

    // float weightf = reading / 58;

    Serial.print("Calibration Factor : ");
    Serial.println(C_factor);
    SerialBT.print("Calibration Factor : ");
    SerialBT.println(C_factor);
    SerialBT.println(weight);
    lcd.setCursor(0, 0); // Column 0, Row 0
    lcd.print("C_Factor");
    lcd.print(weight);
    // Serial.print("Force in granm : ");
    // Serial.print(weightf, 1);
    // Serial.print("gram");
    // Serial.print(" | Force : ");
    // Serial.print(weight, 1);
    // Serial.println("N");
    // SerialBT.print("Force : ");
    // SerialBT.print(weight, 1);
    // SerialBT.println("N");
    // if(weight > 15.0){
    //   digitalWrite(led, HIGH);
    //   delay(20000);
    //   digitalWrite(led, LOW);
    // }
    
  } else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}
