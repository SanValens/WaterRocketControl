#include <Wire.h>

#define mpuAdd 0x68

float accel_raw[3], accel_data[3];
float offset_raw[] = {0.0,0.0,0.0};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial) {}


  Wire.begin();
  delay(250);
  Wire.setClock(400000);

  //Select powermode
  Wire.beginTransmission(mpuAdd);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  //Set DLPF
  Wire.beginTransmission(mpuAdd);
  Wire.write(0x1A);
  Wire.write(0x3);
  Wire.endTransmission();

  //Set Accelerometer range
  Wire.beginTransmission(mpuAdd);
  Wire.write(0x1C);
  Wire.write(0x18); //24 in decimal and 18 on hexadecimal
  //to set the acccelerometer range to +/-16g. CHANGING THIS CHANGES THE CALIBRATION AND COOKING CODES
  Wire.endTransmission();

  calibrate();
}

void calibrate() {
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
    offset_raw[j] = sum[j] / counter;
  }
  offset_raw[2] = -2048.0 + offset_raw[2];
  Serial.println("Calibration finished");
}

void accel_update() {
  Wire.beginTransmission(mpuAdd);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(mpuAdd, 6);
  if(Wire.available() == 6) {
    for(int i = 0; i < 3; i ++) {
      accel_raw[i] = float(Wire.read() << 8 | Wire.read()) - offset_raw[i];
      accel_data[i] = accel_raw[i] / 2048.0;
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  accel_update();
  for(int i = 0; i < 3; i++) {
    Serial.print(accel_data[i]);
    Serial.print("\t");
  }
  Serial.println();
  delay(10);
  //while(true){}
}
