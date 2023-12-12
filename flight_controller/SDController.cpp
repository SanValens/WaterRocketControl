#include <Arduino.h>
#include "SDController.h"
#include <SD.h>


const String FILENAMEROOT = "FLGTDT";
int filecount = 0;
File dataFile;
String file_name = "FLGTDT.csv";

SDController::SDController() {

}

void SDController::begin() {
  while (!SD.begin(sspin)) {}
  delay(1000);
  createFile();
  delay(1000);
}

void SDController::createFile() {
  delay(100);
  while (SD.exists(file_name)) {
      filecount++;
      if (filecount < 10) {
        file_name = FILENAMEROOT + "0" + filecount + ".csv";
      } else {
        file_name = FILENAMEROOT + filecount + ".csv";
      }
      delay(30);
    }
    dataFile = SD.open(file_name, FILE_WRITE);
    dataFile.close();
    delay(2000);

}


void SDController::data_save(char value[60]) {
  dataFile = SD.open(file_name, FILE_WRITE);
  dataFile.println(value);
  dataFile.close();
}

bool SDController::data_check() {
  dataFile = SD.open(file_name, FILE_WRITE);
  if(SD.exists(file_name) && dataFile.size() > 150) {
    dataFile.close();
    return true;
  } else {
    return false;
  }
}