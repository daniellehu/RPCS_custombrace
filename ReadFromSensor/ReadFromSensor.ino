#include <Wire.h>
#define OUTPUT_MIN 1638
#define OUTPUT_MAX 14746
#define PRESSURE_MIN 0
#define PRESSURE_MAX 34.5
#define DECIMALS 4
#define CALIBRATION_OFFSET 0  // figure out some way to do this automatically

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
}

int n = 0;
long arr[4];
long output_P, output_T;
float P = 0, T = 0;

enum STATUS {
  NORMAL,
  COMM,
  STALE,
  DIAG
};

void loop() {
  // put your main code here, to run repeatedly:
  Wire.requestFrom(40, 4);
  Serial.println(n);
  int i = 0;
  while (Wire.available()) {
    arr[i] = (long)Wire.read();
    i++;
  }
  
  output_P = ((arr[0] & 0x3F) << 8) | arr[1];
  output_T = (arr[2] << 3) | (arr[3] >> 5);

  // calculate pressure value
  P = ((output_P - OUTPUT_MIN) * (PRESSURE_MAX - PRESSURE_MIN)) / (OUTPUT_MAX - OUTPUT_MIN) 
        + PRESSURE_MIN + CALIBRATION_OFFSET;

  // calculate temperature value
  T = ((output_T * 200) / 2047) - 50;


  Serial.print(output_P, HEX);
  Serial.print("    ");
  Serial.println(P, DECIMALS);
  Serial.print(output_T, HEX);
  Serial.print("    ");
  Serial.println(T, DECIMALS);
  Serial.println();
  Serial.println();
  n++;
  delay(2000);
}
