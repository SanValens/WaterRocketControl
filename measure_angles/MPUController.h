#ifndef MPUController_h
#define MPUController_h

#include <Arduino.h>
#include <Wire.h>

class MPUController {
    public:
      MPUController();
      void begin(int addrs);
      float accel_raw[3], accel_data[3], gyro_data[3], gyro_raw[3], offset_raw_a[3], offset_raw_g[3];
      int mpuAdd;
      void calibrate_accel();
      void calibrate_gyro();
      void accel_update();
      void gyro_update();
};

#endif