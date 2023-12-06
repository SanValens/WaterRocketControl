#ifndef SDController_h
#define SDController_h

#include <SD.h>
#include <Arduino.h>


class SDController {
  public:
  SDController();
  void begin();
  void createFile();
  void data_save(float value);
  bool data_check();
  private:
  int sspin = 4;
};

#endif