/*

  For more info about this code consult my OneNote Project Structure in the PERSONAL - WATER ROCKET file

*/

#include <Wire.h>


int32_t t_fine;
uint16_t dig_T1, dig_P1; //unsigned
int16_t signed_digT[4]; //dig_T2 to T3, and dig_P2 to P9; Array[0] and array[1] = 0 for simplicity
int16_t signed_digP[10]; //dig_P2 to P9; Array[0] and array[1] = 0 for simplicity
//VALUES IN THE ARRAY ARE NORMALY NUMERATED WITH THE DATASHEET NUMBER OF COEFFICIENT e.g digP4 = signed_digP[4]


#define bmp280add 0x76

int32_t rawTemp;  

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial) {
  }
  Serial.println("Cheap bmp280");

  Wire.begin();
  delay(250);

  //Initializes configuration of oversamplings and power
  Wire.beginTransmission(bmp280add);
  Wire.write(0xF4);
  Wire.write(0x53); //0x53 to set x2 Temp-oversampling; x8 pressure-oversampling and normal power-mode 
  Wire.endTransmission();

  Wire.beginTransmission(bmp280add);
  Wire.write(0xF5);
  Wire.write(0x48); //Set standby to 125ms and filter to 8. Finally to 00 bits we ignore cause they are for serial communication
  Wire.endTransmission();

  delay(100);

  readCoefficients();
  readTemperature();
}

void readCoefficients() {
  Wire.beginTransmission(bmp280add);
  Wire.write(0x88);
  Wire.endTransmission();
  Wire.requestFrom(bmp280add, 24);
  if(Wire.available() == 24) {
    //Different structure than as usual bc the MSB is the second byte, not the first one:
    dig_T1 = int16_t(Wire.read()) | int16_t(Wire.read()) << 8; 
    for(int i = 2; i < 4; i++) {
      signed_digT[i] = int16_t(Wire.read()) | int16_t(Wire.read()) << 8;
    }
    dig_P1 = int16_t(Wire.read()) | int16_t(Wire.read()) << 8; 
        for(int i = 2; i < 10; i++) {
      signed_digP[i] = int16_t(Wire.read()) | int16_t(Wire.read()) << 8;
    }
  }
}

float readTemperature() {
  Wire.beginTransmission(bmp280add);
  Wire.write(0xFA);
  Wire.endTransmission();
  Wire.requestFrom(bmp280add, 3);
  if(Wire.available() == 3) {
    /*  
        * We call uint32_t to be able to translate 16 bit left-wise on every byte. 
        It has to be unsigned cause that's how the data is given according to the datasheet
        * As XLSB has only 4 bits, and as according to the Datasheet the data-read-out 
        gives a 20-bit integer, we move back our 24-bit 
        integer so it becomes a 20-bit one with >>=4 (move all bits right-wise 4 positions)
    */
    rawTemp = (uint32_t(Wire.read()) << 16 | uint32_t(Wire.read()) << 8 | uint32_t(Wire.read())) >> 4;
    int32_t var1, var2, T;
    var1 = ((((rawTemp >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)signed_digT[2])) >> 11;
    var2 = (((((rawTemp >> 4) - ((int32_t)dig_T1)) * ((rawTemp >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)signed_digT[3])) >>14;

    t_fine  = var1 + var2;

    T = (t_fine * 5 + 128) >> 8;
    return float(T) / 100;
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(readTemperature());
  delay(200);
}