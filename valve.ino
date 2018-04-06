uinoconst int PIN = 13;

void setup() {
  pinMode(PIN, OUTPUT);
}

void turnOn() {
  digitalWrite(PIN, HIGH);
}

void turnOff() {
  digitalWrite(PIN, LOW);
}
 
void loop() {
  turnOn();   // turn the LED on (HIGH is the voltage level)
  delay(3000);                       // wait for a second
  turnOff();    // turn the LED off by making the voltage LOW
  delay(3000);                       // wait for a second
}

