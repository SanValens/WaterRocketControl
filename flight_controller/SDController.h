#ifndef SDController_h
#define SDController_h

#include <SD.h>
#include <Arduino.h>


class SDController {
  public:
  SDController();
  void begin();
  void createFile();
  void data_save(char messg[]);
  private:
  int sspin = 9;
};

#endif