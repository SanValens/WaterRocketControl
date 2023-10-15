#include "AltimeterController.h"
#include "MPUController.h"


#define addrs_bmp280 0x76
#define addrs_mpu6050 0x68

AltimeterController bmp280;
MPUController mpu6050;

void setup() {
  Serial.begin(9600);
  while(!Serial) {
    Serial.println("Error in serial initializing");
  }
  // put your setup code here, to run once:
/*   while(!){
    Serial.println("Error in connecting BMP280");
  }; */
  bmp280.begin(addrs_bmp280);
  mpu6050.begin(addrs_mpu6050);
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(bmp280.readTemperature(0));
  delay(50);
}
