#include <Servo.h>

Servo parachute_servo;
int pos  = 0;
int ledPin = 2;

void setup() {
  // put your setup code here, to run once:
  parachute_servo.attach(3);
  delay(10000);
  parachute_servo.write(10);
  delay(10000);
  parachute_servo.write(150);
}

void loop() {

}
