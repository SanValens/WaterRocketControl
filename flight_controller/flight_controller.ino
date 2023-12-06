#include <Wire.h>
#include "AltimeterController.h"
#include "MPUController.h"
#include "SDController.h"

AltimeterController bmp;
MPUController mpu;
SDController sdm;
String condensed_data;


#define addrs_bmp 0x76
#define addrs_mpu 0x68

//#define servo_pin ??

const int tilt_switch_in_pin = 2;
const int red_rgb = 5;
const int green_rgb = 6;
const int blue_rgb = 7;
const int sspin = 4;

const int info_lenght = 40;
char info[info_lenght];

void setup() {
  Serial.begin(9600);
  //pinMode(servo_pin, OUTPUT);
  pinMode(red_rgb, OUTPUT);
  pinMode(green_rgb, OUTPUT);
  pinMode(blue_rgb, OUTPUT);
  delay(1000);
  while (!Serial) {
    Serial.println("Error in serial initializing");
  }
  ledColor(1, 0, 0);  //RED
  sdm.begin();
  ledColor(0, 0, 1);  //BLUE
  pinMode(tilt_switch_in_pin, INPUT_PULLUP);
  sdm.data_save("");
  sdm.data_save("FLIGHT DATA 11/04/2022");
  sdm.data_save("Time(ms), Angle X(°)");
  bmp.begin(addrs_bmp);
  mpu.begin(addrs_mpu);
  delay(1000);
  flight_mode();
}

void ledColor(int r, int g, int b) {
  digitalWrite(red_rgb, r);
  digitalWrite(green_rgb, g);
  digitalWrite(blue_rgb, b);
}

void flight_mode() {
  ledColor(0, 1, 0);  //GREEN
  while(digitalRead(tilt_switch_in_pin)) {}
  ledColor(1, 0, 1);  //PURBLE
  delay(1000);
  long last_time = millis(), init_time = millis();
  int interval_length = 60;
  while (digitalRead(tilt_switch_in_pin)) {
    if (millis() - last_time > interval_length) {
      long time = millis();
      mpu.update_angle_by_kalman_filter();
      mpu.accel_update();
      mpu.gyro_update();
      float pressure = bmp.readPressure();
      float acc_z = mpu.accel_data[2];
      float ang_vel_x = mpu.gyro_data[0];
      float ang_kal_x = mpu.angle_data_by_kalman[0];
      snprintf(info, info_lenght, "%ld,%d,%ld.%03d,%s%d.%03d,%s%d.%03d,%s%d.%03d", time - init_time, int(time - last_time), long(pressure), (int((pressure - long(pressure)) * 1000)), acc_z < 0 ? "-" : "", abs(int(mpu.accel_data[2])), abs((int)(1000*(acc_z -int(acc_z)))),ang_vel_x < 0 ? "-" : "", abs(int(ang_vel_x)), abs(int(1000*(ang_vel_x - int(ang_vel_x)))),ang_kal_x < 0 ? "-" : "", abs(int(ang_kal_x)), abs(int(1000*(ang_kal_x - int(ang_kal_x)))));
      sdm.data_save(info);
      last_time = millis();
    }
  }
  sdm.data_save("RECORDING KILLED");
  delay(10);
  if (sdm.data_check()) {
    ledColor(0, 1, 1);  //Aguamarina
  } else {
    ledColor(1, 0, 0);  //Rojo
  }
}

void loop() {
}