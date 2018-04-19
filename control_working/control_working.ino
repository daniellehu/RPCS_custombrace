
#include "Custom_Brace.h"
#define TOLERANCE 0.4

Custom_Brace cb;
float refP = 3.0;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  if(!cb.sysSetup()) {
    Serial.println("System setup failed");
  }
  Serial.println("System setup succeeded.");
}

void loop() {
  float readP = cb.checkMsg();
  float curP = 0;
  
  if(readP > 0){
    refP= readP;
    Serial.print("Reference pressure received: ");
    Serial.println(refP);
  }
  else{
    Serial.println("Default pressure: 3.0");
  }
  
  //Serial.print("zero P: ");
  //Serial.println(curP);
  for (int i = 0; i < 8; i++)
  {
    curP = curP + cb.readPressure();
    //Serial.print("Addition Pressure: ");
    //Serial.println(curP);
  }
  curP = curP/8;
  Serial.print("Starting pressure: ");
  Serial.println(curP);
  int result;
  if (refP > 0) {
    result = inRange(curP, refP);
    while (result != 0) {
//      Serial.print("Current Pressure: ");
//      Serial.println(curP);
      if (result > 0) {
        // deflate
        cb.openValve(200);
      }
      else if (result < 0) {
        // inflate
        cb.pumpUp(200, 0);
      }
      else {
        break;
      }
      curP = 0;
      for (int i = 0; i < 8; i++) {
        curP = curP + cb.readPressure();
      }
      curP = curP/8;
//      Serial.print("Regulating, current pressure: ");
//      Serial.println(curP);
      result = inRange(curP, refP);
    }
    Serial.println("Done regulating.");
    Serial.print("Final Pressure: ");
    Serial.println(curP);
    Serial.println();
    Serial.println();
  }
  delay(5000);
}

int inRange(float curP, float refP) {
  float margin = 0.4;
  if (curP >= refP - margin && curP <= refP + margin) {
    return 0;
  }
  else if (curP < refP - margin) {
    return -1;
  }
  else {
    return 1;
  }
//  return (abs(curP - refP) <= TOLERANCE);
}

