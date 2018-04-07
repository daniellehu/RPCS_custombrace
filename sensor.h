#include <Wire.h>
#define OUTPUT_MIN 1638
#define OUTPUT_MAX 14745
#define PRESSURE_MIN 0
#define PRESSURE_MAX 34.5
#define DECIMALS 4
#define CALIBRATION_OFFSET 0.0553  

#define DEVICE 40
// figure out some way to do this automatically


// setup SPI connection
void setupSensor() {
    Wire.begin();
}

// read pressure value in kpa
float readPressure(byte device = DEVICE) {
    long arr[2];
    long output_P;
    float P;

    Wire.requrestFrom(device, 2);
    while (!Wire.available())
        delay(1);

    // read values
    for (int i = 0; i < 2; i++) {
        if (Wire.available())
            arr[i] = (long)Wire.read();
        else
            break;
    }

    output_P = ((arr[0] & 0x3F) << 8) | (arr[1] & 0xFF);

    P = ((output_P - OUTPUT_MIN) * (PRESSURE_MAX - PRESSURE_MIN)) / (OUTPUT_MAX - OUTPUT_MIN) 
        + PRESSURE_MIN + CALIBRATION_OFFSET;

    return P;
}

// read temperature value in celcius
float readTemp(byte device = DEVICE) {
    long arr[4];
    long output_T;
    float T;

    Wire.requrestFrom(device, 4);
    while (!Wire.available())
        delay(1);

    // read values
    for (int i = 0; i < 3; i++) {
        if (Wire.available())
            arr[i] = (long)Wire.read();
        else
            break;
    }

    output_T = ((arr[2] & 0xFF) << 3) | ((arr[3] & 0xE0) >> 5);

    T = ((output_T * 200) / 2047) - 50;

    return T;
}