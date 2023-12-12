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
float max_pressure = 0;

char info_temp_press[90] = "";

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
  sdm.data_save("FLIGHT DATA");
  sdm.data_save("Time(ms), T(degC), P(Pa), a-X(g), a-Y(g), a-Z(g), w-X (deg/s), w-Y (deg/s), w-Z (deg/s), Angle-X (deg), Angle-Y(deg)");
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

  //A little pre-update to fresh-up initial values
  mpu.accel_update();
  mpu.gyro_update();
  mpu.update_angle_by_kalman_filter();
  delay(1000);

  long last_time = millis(), init_time = millis();
  int interval_length = 45;
  while (digitalRead(tilt_switch_in_pin)) {
    if (millis() - last_time > interval_length) {
      long time = millis();
      mpu.update_angle_by_kalman_filter();
      mpu.accel_update();
      mpu.gyro_update();
      float temp = bmp.readTemperature(0);
      float pressure = bmp.readPressure();
      snprintf(info_temp_press, sizeof(info_temp_press), "%ld,%d,%d.%02d,%ld.%02d", (int(temp)), (int)(100 * (temp - int(temp))), long(pressure), (int)(100 * (pressure - long(pressure))));
      for (int i = 0; i < 3; i++) {
        snprintf(info_temp_press, sizeof(info_temp_press), "%s,%s%d.%02d", info_temp_press, mpu.accel_data[i] < 0 ? "-" : "", abs(int(mpu.accel_data[i])), abs((int)(100 * (mpu.accel_data[i] - int(mpu.accel_data[i])))));
      }
      for (int i = 0; i < 3; i++) {
        snprintf(info_temp_press, sizeof(info_temp_press), "%s,%s%d.%02d", info_temp_press, mpu.gyro_data[i] < 0 ? "-" : "", abs(int(mpu.gyro_data[i])), abs((int)(100 * (mpu.gyro_data[i] - int(mpu.gyro_data[i])))));
      }
      for (int i = 0; i < 2; i++) {
        snprintf(info_temp_press, sizeof(info_temp_press), "%s,%s%d.%02d", info_temp_press, mpu.angle_data_by_kalman[i] < 0 ? "-" : "", abs(int(mpu.angle_data_by_kalman[i])), abs((int)(100 * (mpu.angle_data_by_kalman[i] - int(mpu.angle_data_by_kalman[i])))));
      }
      sdm.data_save(info_temp_press);
      memset(info_temp_press, 0, sizeof(info_temp_press));
      if(pressure < max_pressure || max_pressure == 0){
        max_pressure = pressure;
      } else if(pressure - max_pressure > 7) {
        ledColor(1,1, 0);  //WHITE
      }
      last_time = time;
    }
  }
  sdm.data_save("RECORDING KILLED");
  Serial.println("HOLA");
  delay(10);
  if (sdm.data_check()) {
    ledColor(0, 1, 1);  //Aguamarina
  } else {
    ledColor(1, 0, 0);  //Rojo
  }
}

void loop() {
}