#ifndef MPUController_h
#define MPUController_h

#include <Arduino.h>
#include <Wire.h>

class MPUController {
    public:
      MPUController();
      void begin(int addrs);
      int mpuAdd;
      void calibrate_accel();
      void calibrate_gyro();
      void accel_update();
      void gyro_update();
      void angle_update_by_acc();
      bool angle_update_by_gyro();
      void update_angle_by_kalman_filter();
      float angle_data_by_kalman[2] = {0,0}; //x y y, respectivamente. Para el video PITCH AND ROLL.
      float angle_data_by_acc[2]; //Calculated using the angle equation seen in the update_angle_acc function
      float angle_data_by_gyro[3] = {0,0,0}; //
      float accel_data[3], gyro_data[3];
    private: 
      float accel_raw[3], gyro_raw[3], offset_raw_a[3], offset_raw_g[3];
      long lastMillis = 0, last_millis_kalman = 0;
      int vertical_axis = 2; //Saves the vertical axis 0=x, 1=y, 2=z. Its recognized in the calibration function
      //variables para filtro de Kalman:
      float incerteza_kalman[2] = {2*2,2*2}; //x y y, respectivamente. Para el video PITCH AND ROLL.
      //X y Y, en ese orden
};

#endif