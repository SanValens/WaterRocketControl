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
  Serial.println("Initializing SD Card...");
  while (!SD.begin(sspin)) {}
  Serial.println("SD Card initialized");
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
    Serial.print("I just created a file called: ");
    Serial.println(file_name);
    dataFile = SD.open(file_name, FILE_WRITE);
    dataFile.close();
    delay(2000);

}


void SDController::data_save(char messg[]) {
  dataFile = SD.open(file_name, FILE_WRITE);
  Serial.println(messg);
  dataFile.println(messg);
  dataFile.close();
}