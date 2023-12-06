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
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  while (!Serial) {
    Serial.println("Error in serial initializing");
  }
  pinMode(9, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  while (!SD.begin(9)) {}

  pinMode(tilt_switch_in_pin, INPUT_PULLUP);
  bmp.begin(addrs_bmp);
  mpu.begin(addrs_mpu);
  sdm.begin();
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
  delay(1000);
  sdm.data_save("-------------------");
  sdm.data_save("FLIGHT DATA 11/04/2022");
  sdm.data_save("Time(ms), Angle X(°)");
}

void loop() {
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH);
  while(digitalRead(tilt_switch_in_pin)) {
    mpu.update_angle_by_kalman_filter();
    Serial.println(mpu.angle_data_by_kalman[0]);
    delay(10);
    //sdm.data_save(String(mpu.angle_data_by_kalman[0])); 
  }
  Serial.println("Code stopped");
  digitalWrite(3, HIGH);
  digitalWrite(5, HIGH);
  while(1){};
}