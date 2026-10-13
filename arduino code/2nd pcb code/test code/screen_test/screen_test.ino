#include <Arduino.h>

#define NOT_SHD_ENABLE PB_2

void setup() {
  pinMode(NOT_SHD_ENABLE, OUTPUT);
}

void loop() {
  digitalWrite(NOT_SHD_ENABLE, LOW);   // should be solid 0 V
  delay(1500);

  digitalWrite(NOT_SHD_ENABLE, HIGH);  // should be solid 3.3 V
  delay(500);
}