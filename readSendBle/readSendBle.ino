// collect data and send through bluetooth upon request

#include <Wire.h>
#define OUTPUT_MIN 1638
#define OUTPUT_MAX 14745
#define PRESSURE_MIN 0
#define PRESSURE_MAX 34.5
#define DECIMALS 4
#define CALIBRATION_OFFSET 0.0553  
#define VALVE_PIN 11
#define SENSOR_PIN 10
#define PUMP_PIN 9
// figure out some way to do this automatically


/*************************** From bleuart example ************************/
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

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}
/***********************************************************************/

void setup() {
  // basic setup
  Wire.begin();
  Serial.begin(9600);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(SENSOR_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);
  

  // ble setup
  
  /* Initialise the module */
  Serial.println("Initialising the Bluefruit LE module: ");

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println("OK!");
//
//  if ( FACTORYRESET_ENABLE )
//  {
//    /* Perform a factory reset to make sure everything is in a known state */
//    Serial.println(F("Performing a factory reset: "));
//    if ( ! ble.factoryReset() ){
//      error(F("Couldn't factory reset"));
//    }
//  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("******************************"));
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
    Serial.println(F("******************************"));
  }

}


void loop() {
  if (checkMsg()) {
    closeValve();
    delay(100);
    for (int i = 0; i < 3; i++) {
      float P = pumpUp(1500, 1000);
      sendMsg(P);
    }
    delay(2000);
    openValve();
    delay(10000);
  }
  delay(100);
}


float pumpUp(int pumpT, int stopT) {
  closeValve();
  digitalWrite(PUMP_PIN, HIGH);
  delay(pumpT);
  digitalWrite(PUMP_PIN, LOW);
  float P = readPressure();
  delay(stopT);
  return P;
}

void openValve() {
  digitalWrite(VALVE_PIN, HIGH);
}

void closeValve() {
  digitalWrite(VALVE_PIN, LOW);
}

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

  // print to serial monitor
  printReadings(output_P, P);

  return P;
}


// print readings from sensor
void printReadings(long output_P, float P) {
  Serial.print(output_P, HEX);
  Serial.print("    ");
  Serial.println(P, DECIMALS);

  Serial.println();
}


// send the given message through bluetooth
void sendMsg(float P) {
  // Send characters to Bluefruit
  
  // construct message to send
  char msg[BUFSIZE+1];
  String s = String(P, DECIMALS);
  s.toCharArray(msg, BUFSIZE);
  s.concat("\n");
  
  Serial.print("[Send] ");
  Serial.println(msg);

  ble.print("AT+BLEUARTTX=");
  ble.println(msg);

  // check response stastus
  if (! ble.waitForOK() ) {
    Serial.println("Failed to send?");
  }
}


// check for incoming commands
bool checkMsg() {
  ble.println("AT+BLEUARTRX");
  ble.readline();
  if (strcmp(ble.buffer, "OK") == 0) {
    // no data
    return false;
  }
  // Some data was found, its in the buffer
  Serial.print("[Recv] "); Serial.println(ble.buffer);
  ble.waitForOK();
  return true;
}
