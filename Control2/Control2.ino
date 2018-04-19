
#include "Custom_Brace.h"

Custom_Brace cb;
#define TOLERANCE 0.2
#define ITERATION 8  // take average pressure over ITERATION values

void setup() {
  Serial.begin(9600);
  while (!Serial);
  while (!cb.sysSetup()) {
    delay(500);
  }
  Serial.println("System setup succeeded, waiting for defualt pressure.");
  
  // wait till default reference pressure is received
  float refP = cb.checkMsg();
  while (refP < 0) {
    delay(500);
    refP = cb.checkMsg();
  }
  cb.defaultRefP = refP;
}

void loop() {
  float refP = cb.checkMsg();
  float curP = 0;
  int T = 200;

  if (refP >= 0) {
    cb.defaultRefP = refP;
    Serial.print("Reference pressure received: ");
    Serial.println(refP);
  }
  else if (refP < 0) {
    refP = cb.defaultRefP;
    Serial.print("Default pressure: ");
    Serial.println(refP);
  }

  curP = cb.readPressure(ITERATION);
  Serial.print("Start pressure: ");
  Serial.println(curP);

  if (refP > 0) {
    int result = cb.inRange(curP, refP, TOLERANCE);
    while (result != 0) {
      if (result > 0) {
        // deflate
        Serial.println("Deflate...");
        cb.openValve(T);
      }
      else {
        // inflate
        Serial.println("Inflate...");
        cb.pumpUp(T, 0);
      }
      curP = cb.readPressure(ITERATION);
      result = cb.inRange(curP, refP, TOLERANCE);
    }
  }
  else if (refP == 0) {
    curP = cb.readPressure(ITERATION);
    while (cb.inRange(curP, refP, TOLERANCE) != 0) {
      Serial.println("Open valve to deflate...");
      cb.openValve(T*50);
      curP = cb.readPressure(ITERATION);
      Serial.println(curP);
    }
  }
  Serial.println("Done regulating.");
  Serial.print("Final pressure: ");
  Serial.println(curP);
  Serial.println();
  Serial.println();
  delay(5000);
}


