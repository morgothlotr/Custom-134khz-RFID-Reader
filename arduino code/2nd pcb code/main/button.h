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
#pragma once

#include <Arduino.h>

// --- Button Configuration ---
#define BUTTON_1_PIN PA_2
//#define BUTTON_2_PIN PA_0
#define DEBOUNCE_DELAY 50  // ms

enum ButtonEvent {
  BUTTON_NONE,
  BUTTON_PRESSED,
  BUTTON_RELEASED
};

struct Button {
  uint8_t pin;
  unsigned long lastDebounceTime;
  int buttonState;
  int lastButtonState;
};

void button_init(Button* btn, uint8_t pin);

ButtonEvent button_get_event(Button* btn);

bool button_get_state(Button* btn);