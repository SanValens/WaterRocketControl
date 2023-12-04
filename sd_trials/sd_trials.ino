#include <SD.h>
#include <SPI.h>
#include <Servo.h>

#define tilt_switch_in_pin 2
#define buzzerpin 5
#define sspin 9
#define powerButton 6
#define servo_pin 3


const String FILENAMEROOT = "FLGTDT";
int filecount = 0;
long interval_lenght = 1000;  //1 second interval

Servo servo;
Sd2Card card;
File dataFile;
String file_name = "FLGTDT.txt";

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  while (!SD.begin(sspin)) {}
  while (!card.init(SPI_HALF_SPEED, sspin)) {}
  Serial.println("Wiring is correct and a card is present.");
  Serial.println("SD Card initialized");

  pinMode(buzzerpin, OUTPUT);
  pinMode(powerButton, INPUT_PULLUP);
  pinMode(tilt_switch_in_pin, INPUT_PULLUP);
  digitalWrite(buzzerpin, LOW);
  servo.attach(servo_pin);


  //dataFile.println("Time (ms), Altitude(m) EQ1, Altitude(m) EQ2, Temperature(°C), Pressure(HPa), X Force (N), Y Force (N), Z Force (N),Rotation X ACC, Rotation Y ACC, Rotation Z ACC, Rotation X GYR,Rotation Y GYR,Rotation Z GYR,Rotation X KALMAN, Rotation Y KALMAN, Rotation Z KALMAN");
  delay(1000);
  createFile();
  delay(1000);
  trial();
}

void createFile() {
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
  dataFile.println("-------------------");
  dataFile.println("FLIGHT DATA 11/04/2022");
  dataFile.println("Time(ms), Contact (Y/N)");
  dataFile.close();
}

void trial() {

}


void loop() {
}
