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

void button_init(Button* btn, uint8_t pin) {
  btn->pin = pin;
  btn->lastDebounceTime = 0;
  btn->buttonState = LOW;
  btn->lastButtonState = LOW;
  
  pinMode(pin, INPUT_PULLDOWN);
}

ButtonEvent button_get_event(Button* btn) {
  int reading = digitalRead(btn->pin);

  //state change
  if (reading != btn->lastButtonState) {
    btn->lastDebounceTime = millis();
  }

  //debounce
  if ((millis() - btn->lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != btn->buttonState) {
      btn->buttonState = reading;
      
      btn->lastButtonState = reading;
      
      return (btn->buttonState == HIGH) ? BUTTON_PRESSED : BUTTON_RELEASED;
    }
  }

  btn->lastButtonState = reading;
  return BUTTON_NONE;
}

bool button_get_state(Button* btn) {
  return btn->buttonState == HIGH;
}