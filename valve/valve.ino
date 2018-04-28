int PIN = 11;

void setup() {
  pinMode(PIN, OUTPUT);
  Serial.begin(9600);
}

void turnOn() {
  digitalWrite(PIN, HIGH);
}

void turnOff() {
  digitalWrite(PIN, LOW);
}
 
void loop() {
  Serial.println("Open");
  turnOn();   // turn the LED on (HIGH is the voltage level)
  delay(5000);                       // wait for a second
  Serial.println("Close");
  turnOff();    // turn the LED off by making the voltage LOW
  delay(5000);                       // wait for a second
}

