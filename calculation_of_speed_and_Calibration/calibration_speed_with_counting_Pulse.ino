const int hallPin = 19;
volatile int triggerCount = 0;
volatile unsigned long lastTrigger = 0;
unsigned long calibrationTime = 15000; // 15 seconds
unsigned long startTime;
bool lastState = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(hallPin, INPUT_PULLUP);
  
  Serial.println("=== A3144 Hall Sensor Calibration (ESP32) ===");
  Serial.println("Move magnet near/away from sensor repeatedly");
  Serial.println("Pin 19 configured with internal pullup");
  
  startTime = millis();
}

void loop() {
  bool currentState = digitalRead(hallPin);
  
  // Detect state changes
  if (currentState != lastState) {
    unsigned long currentTime = millis();
    
    if (currentState == LOW && (currentTime - lastTrigger > 50)) { // 50ms debounce
      triggerCount++;
      lastTrigger = currentTime;
      
      Serial.print("Trigger #");
      Serial.print(triggerCount);
      Serial.print(" at ");
      Serial.print(currentTime - startTime);
      Serial.println("ms - Magnet detected!");
    }
    lastState = currentState;
  }
  
  // Show real-time sensor state
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    Serial.print("Current state: ");
    Serial.println(currentState ? "HIGH (no magnet)" : "LOW (magnet present)");
    lastPrint = millis();
  }
  
  // End calibration
  if (millis() - startTime > calibrationTime) {
    Serial.println("\n=== Calibration Results ===");
    Serial.print("Total triggers: ");
    Serial.println(triggerCount);
    Serial.println(triggerCount > 0 ? "✓ Sensor working properly" : "✗ Check connections");
    Serial.println("Calibration complete - ready for speed measurement");
    
    while(1) delay(1000); // Stop here
  }
}