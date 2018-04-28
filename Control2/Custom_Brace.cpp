// library for the custom brace system


#include <Wire.h>
// #include <stdio.h>
#include "Custom_Brace.h"
//#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

#define OUTPUT_MIN 1638
#define OUTPUT_MAX 14745
#define PRESSURE_MIN 0
#define PRESSURE_MAX 34.5
#define DECIMALS 4
#define CALIBRATION_OFFSET 0
#define VALVE_PIN 11
#define PUMP_PIN 9

#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"


Custom_Brace::Custom_Brace() :
 ble(BLUEFRUIT_SPI_CS, 
      BLUEFRUIT_SPI_IRQ, 
      BLUEFRUIT_SPI_RST)
 {
}

// setup i/o pins and bluetooth
bool Custom_Brace::sysSetup() {
  Wire.begin();
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);

  // BLE setup
  if (!ble.begin(VERBOSE_MODE)) {
    return false;
  }

  ble.echo(false);

  ble.info();

  ble.verbose(false);

  while(!ble.isConnected());

  if (ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION)) {
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  return true;
}

// turn on pump for pumpT time and stop for stopT time
void Custom_Brace::pumpUp(int pumpT, int stopT) {
  closeValve();
  digitalWrite(PUMP_PIN, HIGH);
  delay(pumpT);
  digitalWrite(PUMP_PIN, LOW);
  delay(stopT);
}

// open the valve for time T to deflate air bag
void Custom_Brace::openValve(int T) {
  digitalWrite(VALVE_PIN, HIGH);
  delay(T);
  digitalWrite(VALVE_PIN, LOW);
}

// close valve to seal air flow between pump and air bag
void Custom_Brace::closeValve() {
  digitalWrite(VALVE_PIN, LOW);
}

// read the pressure value from sensor
float requestPressure() {
  long arr[2];
  long output_P;
  float P = 0;
  
  Wire.requestFrom(40, 2);
  int i = 0;
  while (Wire.available()) {
    arr[i] = (long)Wire.read();
    i++;
  }
  
  output_P = ((arr[0] & 0x3F) << 8) | arr[1];

  // calculate pressure value
  P = ((output_P - OUTPUT_MIN) * (PRESSURE_MAX - PRESSURE_MIN)) / (OUTPUT_MAX - OUTPUT_MIN) 
        + PRESSURE_MIN + CALIBRATION_OFFSET;

  return P;
}

// get the average pressure over ITERATION values
float Custom_Brace::readPressure(int n) {
  float result = 0;
  for (int i = 0; i < n; i++) {
    result += requestPressure();
  }
  result /= n;
  return result;
}

// send the pressure value through bluetooth
bool Custom_Brace::sendMsg(char msg[]) {

  ble.print("AT+BLEUARTTX=");
  ble.println(msg);

  return ble.waitForOK();
}

// check for incoming command (reference pressure)
// sending -10 from phone means deflation, returns 0
// returns the reference pressure if there is one, -1 if no data
// or something goes wrong
float Custom_Brace::checkMsg() {
  float P;
  ble.println("AT+BLEUARTRX");
  ble.readline();
  if (strcmp(ble.buffer, "OK") == 0) {
    // no data
    return -1;
  }

  // some data was found, its in the buffer
  P = atof(ble.buffer);

  if (!ble.waitForOK()) {
    return -1;
  }

  if (P == -10) {
    return 0;
  }

  return P;
}

// check whether the current pressure is within the acceptable range around
// the reference pressure
int Custom_Brace::inRange(float curP, float refP, float margin) {
  float diff = curP - refP;
  if (abs(diff) <= margin) {
    return 0;
  }
  else if (diff < 0) {
    return -1;
  }
  else {
    return 1;
  }
}
