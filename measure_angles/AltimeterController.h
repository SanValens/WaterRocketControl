#ifndef AltimeterController_h
#define AltimeterController_h

#include <Arduino.h>
#include <Wire.h>


class AltimeterController
{
    public:
        AltimeterController();
        void begin(int addrs);
        void getReferencePressure();
        void readCoefficients();
        float readTemperature(bool for_pressure);
        float readPressure();
        float readAbsAltitude();
        float readRelAltitude();
        uint16_t dig_T1, dig_P1; //unsigned
    private:
        int _bmp280add;
        int32_t t_fine, rawTemp, rawPressure;
        int16_t signed_digT[4]; //dig_T2 to T3, and dig_P2 to P9; Array[0] and array[1] = 0 for simplicity
        int16_t signed_digP[10]; //dig_P2 to P9; Array[0] and array[1] = 0 for simplicity
        //VALUES IN THE ARRAY ARE NORMALY NUMERATED WITH THE DATASHEET NUMBER OF COEFFICIENT e.g digP4 = signed_digP[4]
        float referencePressure;
        
};

#endif