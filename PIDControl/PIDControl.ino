
#include "Custom_Brace.h"
#define TOLERANCE 0.4

Custom_Brace cb;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  if(!cb.sysSetup()) {
    Serial.println("System setup failed");
  }
  Serial.println("System setup succeeded.");
}

void loop() {
  float refP = cb.checkMsg();
  float curP = cb.readPressure();
  int result;
  Serial.print("Pressure received: ");
  Serial.println(refP);
  if (refP > 0) {
    result = inRange(curP, refP);
    while (result != 0) {
      Serial.print("Current Pressure: ");
      Serial.println(curP);
      if (result > 0) {
        // deflate
        cb.openValve(500);
      }
      else {
        // inflate
        cb.pumpUp(500, 0);
      }
      curP = cb.readPressure();
      result = inRange(curP, refP);
    }
    Serial.println("Done regulating.");
    Serial.println();
    Serial.println();
  }
}

int inRange(float curP, float refP) {
  if (curP == refP) {
    return 0;
  }
  else if (curP < refP) {
    return -1;
  }
  else {
    return 1;
  }
//  return (abs(curP - refP) <= TOLERANCE);
}

