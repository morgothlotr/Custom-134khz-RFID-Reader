#include "toggle_pin.h"

void toggleInit() {
  pinMode(TOGGLE_ADDRESS, OUTPUT);
}

void togglePin() {
  static bool ledState = false;
  ledState = !ledState;
  digitalWrite(TOGGLE_ADDRESS, ledState);
}