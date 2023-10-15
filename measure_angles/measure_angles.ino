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
/*   mpu6050.accel_update();
  for(int i = 0; i < 3; i++) {
    Serial.print(mpu6050.accel_data[i]);
    Serial.print("\t");
  }
  Serial.println();
  delay(100); */
  Serial.print("Altura respecto al nivel de mar: ");
  Serial.println(bmp280.readAbsAltitude());
}
