#include <Arduino.h>
#include "MPUController.h"
#include <Wire.h>

MPUController::MPUController() {

}

void begin(int addrs) {
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
  Serial.println("Calibration started, leave the device on a flat surface");
  delay(100);
  for(int i = 0; i < 100; i++) {
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
  Serial.println("Calibration finished");
}

void MPUController::calibrate_gyro() {
  Serial.println("Calibration initialized, hold still");
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
  Serial.println("Calibration finished");
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
