/*

  For more info about this code consult my OneNote Project Structure in the PERSONAL - WATER ROCKET file

*/

#include <Wire.h>
#include <math.h>


int32_t t_fine;
uint16_t dig_T1, dig_P1; //unsigned
int16_t signed_digT[4]; //dig_T2 to T3, and dig_P2 to P9; Array[0] and array[1] = 0 for simplicity
int16_t signed_digP[10]; //dig_P2 to P9; Array[0] and array[1] = 0 for simplicity
//VALUES IN THE ARRAY ARE NORMALY NUMERATED WITH THE DATASHEET NUMBER OF COEFFICIENT e.g digP4 = signed_digP[4]


#define bmp280add 0x76

int32_t rawTemp, rawPressure;

float referenceTemperature = 27.1;
float referencePressure;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial) {
  }
  Serial.println("Cheap bmp280");

  Wire.begin();
  delay(250);
  readCoefficients();

  //Initializes configuration of oversamplings and power
  Wire.beginTransmission(bmp280add);
  Wire.write(0xF4);
  Wire.write(0x57); //0x53 to set x2 Temp-oversampling; x16 pressure-oversampling and normal power-mode 
  Wire.endTransmission();

  Wire.beginTransmission(bmp280add);
  Wire.write(0xF5);
  Wire.write(0x14); //Set standby to sandard and filter to 16w. Finally to 00 bits we ignore cause they are for serial communication
  Wire.endTransmission();

  delay(100);


  getReferencePressure();
}

void getReferencePressure() {
  float pressures = 0;
  int counter = 0;
  for(int i = 0; i < 100; i++) {
    pressures += readPressure();
    counter++;
    delay(50);
  }
  referencePressure = pressures/counter;
  delay(3000);
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

float readTemperature(bool for_pressure) {
  if(for_pressure) {  
    Wire.beginTransmission(bmp280add);
    Wire.write(0xF7);
    Wire.endTransmission();
    Wire.requestFrom(bmp280add, 6);
    rawPressure = (uint32_t(Wire.read()) << 16 | uint32_t(Wire.read()) << 8 | uint32_t(Wire.read())) >> 4;
  } else {
    Wire.beginTransmission(bmp280add);
    Wire.write(0xFA);
    Wire.endTransmission();
    Wire.requestFrom(bmp280add, 3);
  }
  if(Wire.available() >= 3) {
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

float readPressure() {
  readTemperature(1);
  int64_t var1, var2, p;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)signed_digP[6];
  var2 = var2 + ((var1 * (int64_t)signed_digP[5]) << 17);
  var2 = var2 + (((int64_t)signed_digP[4]) << 35);
  var1 = ((var1 * var1 * (int64_t)signed_digP[3]) >> 8) +
         ((var1 * (int64_t)signed_digP[2]) << 12);
  var1 =
      (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;

  if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576 - rawPressure;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)signed_digP[9]) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)signed_digP[8]) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)signed_digP[7]) << 4);

  return (float)p / 256; //Returns altitude in pascals
}

double readAbsAltitude() {
  return (log((readPressure()/101325.0))/log(2.718281828)) * ((readTemperature(0) + 273.15))/(-3.416 / 100);
  //return 44330*(1-pow((readPressure()/101325), 0.19029));
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(readPressure());
  delay(2000);
}
