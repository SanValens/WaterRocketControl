#ifndef MPUController_h
#define MPUController_h

#include <Arduino.h>
#include <Wire.h>

class MPUController {
    public: 
      float accel_raw[3], accel_data[3], gyro_data[3], gyro_raw[3];
      float offset_raw_a[] = {0.0,0.0,0.0};
      float offset_raw_g[] = {0.0,0.0,0.0};
      long timer;
      int mpuAdd;
      void begin(int addrs);
      void calibrate_accel();
      void calibrate_gyro();
      void accel_update();
      void gyro_update();
};

#endif