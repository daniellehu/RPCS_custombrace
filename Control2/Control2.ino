
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
  cb.sendMsg("Please enter default pressure.    ");
  
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
  char msg[BUFSIZE+1];

  cb.sendMsg("[Test] This is working    ");
  if (refP >= 0) {
    cb.defaultRefP = refP;
    Serial.print("Reference pressure received: ");
    Serial.println(refP);
    sprintf(msg, "Reference pressure is %f    ", refP);
    cb.sendMsg(msg);
  }
  else if (refP < 0) {
    refP = cb.defaultRefP;
    Serial.print("Default pressure: ");
    Serial.println(refP);
    sprintf(msg, "Reference pressure is %f    ", refP);
    cb.sendMsg(msg);
  }

  curP = cb.readPressure(ITERATION);
  Serial.print("Start pressure: ");
  Serial.println(curP);
  sprintf(msg, "Starting from %f...", curP);
  cb.sendMsg(msg);
  
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
    cb.sendMsg("Deflate all.\r\n");
    while (cb.inRange(curP, refP, TOLERANCE) != 0) {
      Serial.println("Open valve to deflate...");
      cb.openValve(T*10);
      curP = cb.readPressure(ITERATION);
      Serial.println(curP);
    }
  }
  Serial.println("Done regulating.");
  Serial.print("Final pressure: ");
  Serial.println(curP);
  Serial.println();
  Serial.println();
  sprintf(msg, "to %f    ", curP);
  cb.sendMsg(msg);
  delay(5000);
}



