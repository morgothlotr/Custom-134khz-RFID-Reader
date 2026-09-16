/*
 * 134.2 kHz RFID Animal Tag Reader
 * Copyright (C) 2026 University of Canterbury FYP Group E02
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */
#include "button.h"

//#define SIMULATOR_VCC_PIN PC12

//Debounce 
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // ms
int buttonState = LOW;
int lastButtonState = LOW;

void button_init() {
  pinMode(BUTTON_1_PIN, INPUT_PULLDOWN);
  //pinMode(SIMULATOR_VCC_PIN, OUTPUT);
  //digitalWrite(SIMULATOR_VCC_PIN, HIGH);
}

bool button_is_pressed() {
  int reading = digitalRead(BUTTON_1_PIN);

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
