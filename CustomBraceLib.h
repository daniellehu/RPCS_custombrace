// library for the custom brace system

#include <Wire.h>
#include <CustomBraceLib.c>

#define OUTPUT_MIN 1638
#define OUTPUT_MAX 14745
#define PRESSURE_MIN 0
#define PRESSURE_MAX 34.5
#define DECIMALS 4
#define CALIBRATION_OFFSET 0
#define VALVE_PIN 11
#define PUMP_PIN 9

// setup i/o pins and bluetooth
void sysSetup();

// turn on pump for pumpT time and stop for stopT time
float pumpUp(int pumpT, int stopT = 0);

// open the valve for time T to deflate air bag
void openValve(int T = 0);

// close valve to seal air flow between pump and air bag
void closeValve();

// read the pressure value from sensor
float readPressure();

// send the pressure value through bluetooth
bool sendMsg(float P);

// check for incoming command (reference pressure)
float checkMsg();