#include <Arduino.h>
#include "MPUController.h"
#include <Wire.h>

MPUController::MPUController() {

}



void MPUController::begin(int addrs) {
  for(int i = 0; i < 3; i++) {
    offset_raw_a[i] = 0.0;
    offset_raw_g[i] = 0.0;
  }
  mpuAdd = addrs;
  Wire.begin();
  delay(250);

  //Select powermode
  Wire.beginTransmission(mpuAdd);
  Wire.write(0x6B);
  Wire.write(0x08);
  Wire.endTransmission();

  //Set DLPF  
  Wire.beginTransmission(mpuAdd);  //Starts I2C transmission  for DLPF
  Wire.write(0x1A);              //Pass the register as DATA. MPU6050 understands the byte as a register ID
  Wire.write(0x05);              //Equals: 00000101 bits for the 0x1A register
  Wire.endTransmission();        //Sends data, this sets the DLPF

  //Set gyroscope range 
  Wire.beginTransmission(0x68);  //Starts a new transmission
  Wire.write(0x1B);
  Wire.write(0x8);               //00001000 for setting 1 FS_SEL = 1; giving a full scale range of 500°/s or 65.5LSB/°/s
  Wire.endTransmission();


  //Set Accelerometer range
  Wire.beginTransmission(mpuAdd);
  Wire.write(0x1C);
  Wire.write(0x18); //24 in decimal and 18 on hexadecimal
  //to set the acccelerometer range to +/-16g. CHANGING THIS CHANGES THE CALIBRATION AND COOKING CODES
  Wire.endTransmission();

  calibrate_accel();
  calibrate_gyro();
}


void MPUController::calibrate_accel() {
  float sum[] = {0.0,0.0,0.0};
  float counter = 0.0;
  delay(100);
  for(int i = 0; i < 300; i++) {
    accel_update();
    counter += 1.0;
    for(int j = 0; j < 3; j++) {
      sum[j] = sum[j] + accel_raw[j];
    }
    delay(10);
  }
  for(int j = 0; j < 3; j++) {
    offset_raw_a[j] = sum[j] / counter;
  }
  offset_raw_a[2] = -2048.0 + offset_raw_a[2];
  Serial.println("Acceleration calibration finished");
}

void MPUController::calibrate_gyro() {
  float sum[] = { 0, 0, 0 };
  float counter = 0;
  for (int i = 0; i < 500; i++) {
    counter += 1.0;
    gyro_update();
    for (int j = 0; j < 3; j++) {
      sum[j] = sum[j] + gyro_raw[j];
    }
    delay(10);
  }
  for (int j = 0; j < 3; j++) {
    offset_raw_g[j] = sum[j] / counter;
  }
  Serial.println("Gyroscope calibration finished");
}

void MPUController::accel_update() {
  Wire.beginTransmission(mpuAdd);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(mpuAdd, 6);
  if(Wire.available() == 6) {
    for(int i = 0; i < 3; i ++) {
      accel_raw[i] = float(Wire.read() << 8 | Wire.read()) - offset_raw_a[i];
      accel_data[i] = accel_raw[i] / 2048.0;
    }
  }
}

void MPUController::gyro_update(void) {
  Wire.beginTransmission(0x68);
  Wire.write(0x43);  //Register holding gyroscope, we indicate we are going to use this one
  Wire.endTransmission();

  Wire.requestFrom(0x68, 6);    //Request 6 bytes from Gyroscope measurments register. Meaning 48 bits
  if (Wire.available() == 6) {  //Make sure i now have 6 bytes in buffer
    for (int i = 0; i < 3; i++) {
      gyro_raw[i] = float(Wire.read() << 8 | Wire.read()) - offset_raw_g[i];  //Every .read() consums a byte from the buffer
      gyro_data[i] = gyro_raw[i] / 65.6;
    }
    //Ergo when i call .read() 6 times, i finish with no bytes in the buffer
  }
}

void MPUController::angle_update_by_acc() {
  accel_update();
  angle_data_by_acc[0] = atan(accel_data[1]/sqrt((accel_data[0]*accel_data[0])+(accel_data[2]*accel_data[2]))) * 180/3.141592;
  angle_data_by_acc[1] = -atan(accel_data[0]/sqrt((accel_data[1]*accel_data[1])+(accel_data[2]*accel_data[2]))) * 180/3.141592;
}


bool MPUController::angle_update_by_gyro() {
  if(lastMillis == 0) {
    lastMillis = millis();
    return 0;
  }
  float t = (millis()-lastMillis);
  gyro_update();
  for(int i = 0; i < 3; i++) {
    angle_data_by_gyro[i] = angle_data_by_gyro[i] + (gyro_data[i] * t/1000);
  }
  lastMillis = millis();
}

void MPUController::update_angle_by_kalman_filter() {
  if (last_millis_kalman == 0) {
    last_millis_kalman = millis();
    return 0;
  }
  float t = (millis() - last_millis_kalman);
  gyro_update();
  angle_update_by_acc();
  angle_update_by_gyro();
  //Un poco mas de entendimiento con apartado Kalman Filter en una dimensión en mi cuaderno de programación
  //Ecuaciones de la 1 a la 5:
  for(int i = 0; i < 2; i++){
    angle_data_by_kalman[i] = angle_data_by_kalman[i] + (t / 1000) * gyro_data[i];  //Calculo por integración.
    incerteza_kalman[i] = incerteza_kalman[i] + ((t / 1000) * (t / 1000) * 4 * 4);
    float k_gain = incerteza_kalman[i] * 1 / (1 * incerteza_kalman[i] + 3 * 3);
    angle_data_by_kalman[i] = angle_data_by_kalman[i] + k_gain * (angle_data_by_acc[i] - angle_data_by_kalman[i]);
    incerteza_kalman[i] = (1 - k_gain) * incerteza_kalman[i];
  }

  last_millis_kalman = millis();
}