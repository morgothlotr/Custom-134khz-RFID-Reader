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
#include "pin.h"

#define NUM_PINS 3
// Store state for each pin (supports up to NUM_PINS pins)
struct PinState {
  int pin;
  bool state;
  unsigned long last_toggle;
  bool in_use;
};


static PinState pins[NUM_PINS];
static int pin_count = 0;

void pin_init(int pin) {
  for (int i = 0; i < pin_count; i++) {
    if (pins[i].pin == pin) return;  // Already initialized
  }
  
  pins[pin_count].pin = pin;
  pins[pin_count].state = false;
  pins[pin_count].last_toggle = millis();
  pins[pin_count].in_use = true;
  pin_count++;
  
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void toggle_pin(int pin) {
  for (int i = 0; i < pin_count; i++) {
    if (pins[i].pin == pin) {
      pins[i].state = !pins[i].state;
      digitalWrite(pin, pins[i].state);
      pins[i].last_toggle = millis();
      return;
    }
  }
}

void toggle_pin_update(int pin, unsigned long interval_ms) {
  for (int i = 0; i < pin_count; i++) {
    if (pins[i].pin == pin) {
      unsigned long current_time = millis();
      if (current_time - pins[i].last_toggle >= interval_ms) {
        toggle_pin(pin);  // Reuse immediate toggle
      }
      return;
    }
  }
}

void set_pin(int pin, bool state) {
  for (int i = 0; i < pin_count; i++) {
    if (pins[i].pin == pin) {
      pins[i].state = state;
      digitalWrite(pin, state);
      pins[i].last_toggle = millis();
      return;
    }
  }
}