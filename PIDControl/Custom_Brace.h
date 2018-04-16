// library for the custom brace system

#ifndef _CUSTOM_BRACE_H_
#define _CUSTOM_BRACE_H_


#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

class Custom_Brace {

  private:
    Adafruit_BluefruitLE_SPI ble;

  public:
    // Constructor
    Custom_Brace();

    // setup i/o pins and bluetooth
    bool sysSetup();

    // turn on pump for pumpT time and stop for stopT time
    void pumpUp(int pumpT, int stopT = 0);

    // open the valve for time T to deflate air bag
    void openValve(int T);

    // close valve to seal air flow between pump and air bag
    void closeValve();

    // read the pressure value from sensor
    float readPressure();

    // send the pressure value through bluetooth
    bool sendMsg(float P);

    // check for incoming command (reference pressure)
    float checkMsg();

};

#endif