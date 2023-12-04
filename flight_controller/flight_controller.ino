#include <Wire.h>
#include "AltimeterController.h"
#include "MPUController.h"
#include <SD.h>
#include "SDController.h"

AltimeterController bmp;
MPUController mpu;
SDController sdm;
String condensed_data;


#define addrs_bmp 0x76
#define addrs_mpu 0x68

#define tilt_switch_in_pin 2
#define servo_pin 3


void setup() {
  Serial.begin(9600);
  while (!Serial) {
    Serial.println("Error in serial initializing");
  }

  pinMode(9, OUTPUT);

  Serial.println("Initializing SD Card...");
  while (!SD.begin(9)) {}
  Serial.println("SD Card initialized");

  pinMode(tilt_switch_in_pin, INPUT_PULLUP);

  Serial.println("Initializing calibrations...");
  bmp.begin(addrs_bmp);
  //mpu.begin(addrs_mpu);
  Serial.println("All calibrations finished...");
  sdm.begin();
  delay(1000);
  sdm.data_save("-------------------");
  sdm.data_save("FLIGHT DATA 11/04/2022");
  sdm.data_save("Time(ms), Angle X(°)");
}

void loop() {
}

void stop() {
  Serial.println("Code stopped");
  while(1){}
}