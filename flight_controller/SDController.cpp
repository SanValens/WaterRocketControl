#include <Arduino.h>
#include "SDController.h"
#include <SD.h>


const String FILENAMEROOT = "FLGTDT";
int filecount = 0;
File dataFile;
String file_name = "FLGTDT.txt";

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
        file_name = FILENAMEROOT + "0" + filecount + ".txt";
      } else {
        file_name = FILENAMEROOT + filecount + ".txt";
      }
      delay(30);
    }
    dataFile = SD.open(file_name, FILE_WRITE);
    dataFile.close();
    delay(2000);

}


void SDController::data_save(String messg) {
  dataFile = SD.open(file_name, FILE_WRITE);
  dataFile.println(messg);
  dataFile.close();
}