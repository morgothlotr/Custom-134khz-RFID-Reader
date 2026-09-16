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
#include "pin.h"

Button button1;

// state machine for reader
enum AppState {
  IDLE,
  SCANNING,
  WAKE_START
};
AppState currentState = WAKE_START;

void setup() {
  button_init();
  pin_init(POWER_LED_ADDRESS);
  pin_init(STATUS_LED_ADDRESS);
  button_init(&button1, BUTTON_1_PIN);
}


AppState currentState = IDLE;

void loop() {
  toggle_pin_update(POWER_LED_ADDRESS, 500)
  set_pin(STATUS_LED_ADDRESS, button_get_state(&statusButton));



}
