#include <Wire.h>  //Library used for I2C communication

float gyro_data[3], gyro_raw[3];
float offset_raw[] = { 0.0, 0.0, 0.0 };

long timer;

void gyro_update(void) {
  Wire.beginTransmission(0x68);
  Wire.write(0x43);  //Register holding gyroscope, we indicate we are going to use this one
  Wire.endTransmission();

  Wire.requestFrom(0x68, 6);    //Request 6 bytes from Gyroscope measurments register. Meaning 48 bits
  if (Wire.available() == 6) {  //Make sure i now have 6 bytes in buffer
    for (int i = 0; i < 3; i++) {
      gyro_raw[i] = float(Wire.read() << 8 | Wire.read()) - offset_raw[i];  //Every .read() consums a byte from the buffer
      gyro_data[i] = gyro_raw[i] / 65.6;
    }
    //Ergo when i call .read() 6 times, i finish with no bytes in the buffer
  }
}

void setup() {
  Serial.begin(57600);

  Wire.begin();
  Wire.setClock(400000);
  delay(250);

  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x8);  //Sets powermanagement to normal mode and desables temperature sensor
  Wire.endTransmission();

  Wire.beginTransmission(0x68);  //Starts I2C transmission  for DLPF
  Wire.write(0x1A);              //Pass the register as DATA. MPU6050 understands the byte as a register ID
  Wire.write(0x05);              //Equals: 00000101 bits for the 0x1A register
  Wire.endTransmission();        //Sends data, this sets the DLPF

  Wire.beginTransmission(0x68);  //Starts a new transmission
  Wire.write(0x1B);              //Define the adressee register
  Wire.write(0x8);               //00001000 for setting 1 FS_SEL = 1; giving a full scale range of 500°/s or 65.5LSB/°/s
  Wire.endTransmission();
  calibrate_gyro();
}

void calibrate_gyro() {
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
    offset_raw[j] = sum[j] / counter;
  }
  Serial.println("Calibration finished");
}


void loop() {
  gyro_update();  //Sends variable pointers so it can be changed inside the function without the use of return
  for (int j = 0; j < 3; j++) {
    Serial.print(gyro_data[j]);
    Serial.print("\t");
  }
  Serial.println("");
  delay(10);
}
