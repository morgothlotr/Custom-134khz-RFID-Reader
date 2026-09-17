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

#include "sleep.h"
#include <STM32LowPower.h>
#include "demod_read.h"
#include "oled_screen.h"

#define sleep_timeout_s 60
static uint32_t last_input_time = 0;

void sleep_init() {
  LowPower.begin();
  LowPower.attachInterruptWakeup(WAKE_PIN, nullptr, RISING, SHUTDOWN_MODE);
  sleep_input();
}

void sleep_input() {
  last_input_time = millis() / 1000;
}

bool sleep_check_timeout() {
  uint16_t current_time_s = millis() / 1000;
  if((current_time_s - last_input_time) > sleep_timeout_s) {
    return true;
  }
  return false;
}

void sleep_enter_standby() {
  // do shutdown tasks before sutting down
  shd_disable();
  oled_clear();
  oled_update();
  oled_sleep();
  //TODO need something to ensure writing to storage completes correctly and doesnt stop in the middle

  LowPower.shutdown();  
}