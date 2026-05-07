#include "button.h"

// --- Button Configuration ---
#define BUTTON_PIN PC14
#define SIMULATOR_VCC_PIN PC12

// --- Debounce Logic ---
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // ms
int buttonState = LOW;
int lastButtonState = LOW;

void button_init() {
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(SIMULATOR_VCC_PIN, OUTPUT);
  digitalWrite(SIMULATOR_VCC_PIN, HIGH);
}

bool button_is_pressed() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        return true;
      }
    }
  }

  lastButtonState = reading;
  return false;
}
