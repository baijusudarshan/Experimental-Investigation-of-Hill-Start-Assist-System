#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
 lcd.init(); // Initialize LCD
 lcd.backlight(); // Turn on backlight
}
void loop() {
 lcd.clear();
 lcd.setCursor(0, 0); // Column 0, Row 0
 lcd.print("Arduino");
 lcd.print("LCD I2C Demo");
 delay(2000);
 Serial.print(PI);
}