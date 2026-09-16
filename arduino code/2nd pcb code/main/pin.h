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
#define POWER_LED_ADDRESS PB_1
#define STATUS_LED_ADDRESS PB_0

// Initialize a pin
void pin_init(int pin);

// Toggle immediately
void toggle_pin(int pin);

// Toggle on interval (call every loop)
void toggle_pin_update(int pin, unsigned long interval_ms);

// set pin to value
void set_pin(int pin, bool state);