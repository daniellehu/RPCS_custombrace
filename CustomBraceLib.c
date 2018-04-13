// library for the custom brace system

#include <Wire.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, 
                             BLUEFRUIT_SPI_IRQ, 
                             BLUEFRUIT_SPI_RST);


// setup i/o pins and bluetooth
bool sysSetup() {
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

  while(!ble.isConnected()) {
    delay(500);
  }

  if (ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION)) {
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  return true;
}

// turn on pump for pumpT time and stop for stopT time
void pumpUp(int pumpT, int stopT = 0) {
  closeValve();
  digitalWrite(PUMP_PIN, HIGH);
  delay(pumpT);
  digitalWrite(PUMP_PIN, LOW);
  delay(stopT);
}

// open the valve for time T to deflate air bag
void openValve(int T = 0) {
  digitalWrite(VALVE_PIN, HIGH);
  delay(T);
}

// close valve to seal air flow between pump and air bag
void closeValve() {
  digitalWrite(VALVE_PIN, LOW);
}

// read the pressure value from sensor
float readPressure() {
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

// send the pressure value through bluetooth
bool sendMsg(float P) {
  char msg[BUFSIZE+1];
  String s = Sring(P, DECIMALS);
  s.toCharArray(msg, BUFSIZE);

  ble.print("AT+BLEUARTTX=");
  ble.println(msg);

  return ble.waitForOK();
}

// check for incoming command (reference pressure)
// sending -10 from phone means deflation, returns 0
// returns the reference pressure if there is one, -1 if no data
// or something goes wrong
float checkMsg() {
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