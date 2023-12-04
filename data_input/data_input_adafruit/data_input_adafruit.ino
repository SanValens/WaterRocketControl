#include <Adafruit_MPU6050.h> //This library takes 34% of maximum program storage

#define mpu_address 0x68

Adafruit_MPU6050 mpu;

float x_roll, y_pitch, z_yaw;

long timer;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial) {}

  if(mpu.begin(mpu_address)) {
    Serial.println("MPU CONNECTED");
  } else {
    Serial.println("FAILED TO CONNECT MPU6050");
  }
  //Set DLPF
  mpu.setFilterBandwidth(MPU6050_BAND_10_HZ);
  //Set Range to 500
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
} 

void loop() {
  // put your main code here, to run repeatedly:
  long big_timer = millis();
  for(int counter = 0; counter < 1000; counter ++ ) {
    timer = millis();
    sensors_event_t gData; 
    mpu.getEventGyro(&gData); //Sends variable pointers so it can be changed inside the function without the use of return
    Serial.print(gData.gyro.x);
    Serial.print("\t");
    Serial.print(gData.gyro.y);
    Serial.print("\t");
    Serial.print(gData.gyro.z);
    Serial.print("\t");
    Serial.print(millis() - timer);
    Serial.println( counter);
    delay(10);
  }
  Serial.print("Tiempo total de procesamiento para mil iteraciones: ");
  Serial.print(millis() - big_timer);
  Serial.print("ms");
  while(true) {

  }
}
