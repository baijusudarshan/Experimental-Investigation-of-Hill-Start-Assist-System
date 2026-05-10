const int pin = 19;
const int led = 2;
void setup(){
  Serial.begin(115200);
  pinMode(pin, INPUT_PULLUP);
  pinMode(led, OUTPUT);

}

void loop(){
  int sensorState = digitalRead(pin);
  if(sensorState == LOW){
    digitalWrite(led, HIGH);
    Serial.println("you have on the system");
  }
  else{
   digitalWrite(led, LOW);
   Serial.println("you have off the system");
  }
  delay(100);
}  

// const int SENSOR_PIN = 19; 
// const int led = 2;
// volatile unsigned int pulseCount = 0;
// unsigned long lastMillis = 0;
// // Interrupt Service Routine (ISR)
// void IRAM_ATTR countPulse() {
//   pulseCount++;
// }

// void setup() {
//   Serial.begin(115200);
//   pinMode(SENSOR_PIN, INPUT_PULLUP); 
  // pinMode(led, OUTPUT);
//   attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), countPulse, FALLING);
// }

// void loop() {
//   if(pulseCount == 1){
//     digitalWrite(led, HIGH);
//     pulseCount = 0;
//   }
//   else{
//     digitalWrite(led, LOW);
//   }
//   delay(100);
// }
// const int SENSOR_PIN = 19; 
// const int led = 2;
// volatile unsigned int pulseCount = 0;
// unsigned long lastMillis = 0;

// void IRAM_ATTR countPulse() {
//   pulseCount++;
// }

// void setup() {
//   Serial.begin(115200);
//   pinMode(SENSOR_PIN, INPUT_PULLUP); 
//   pinMode(led, OUTPUT);
  
//   attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), countPulse, FALLING);
// }

// void loop() {
//   if (millis() - lastMillis >= 500) {
    
//     noInterrupts();
//     unsigned int copiesCount = pulseCount;
//     pulseCount = 0;
//     interrupts();
//     if (copiesCount == 1){
//       digitalWrite(led, HIGH)
//     }


//     lastMillis = millis();
//   }
// }