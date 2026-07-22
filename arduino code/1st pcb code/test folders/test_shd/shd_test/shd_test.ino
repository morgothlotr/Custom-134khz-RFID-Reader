#include <Arduino.h>

#define TOGGLE_PIN1 PB_3
#define TOGGLE_PIN2 PA_8

void setup() {
  pinMode(TOGGLE_PIN1, OUTPUT);
  pinMode(TOGGLE_PIN2, OUTPUT);
}

void loop() {
  static bool pinState = false;

  pinState = !pinState;

  digitalWrite(TOGGLE_PIN1, pinState);
  digitalWrite(TOGGLE_PIN2, pinState);

  delay(250);  
}