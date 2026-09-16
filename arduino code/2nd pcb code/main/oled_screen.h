/*
 * 134.2 kHz RFID Animal Tag Reader
 * Copyright (C) 2026 University of Canterbury FYP Group E02
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <Wire.h>
#include "fdx_b_decoder.h"

extern TwoWire OLED_WIRE;

#define OLED_ADDR        0x3C
#define OLED_WIDTH       128
#define OLED_HEIGHT      32
#define OLED_COL_OFFSET  4

// Initialize the OLED and clear its framebuffer.
void oled_init();

// Send framebuffer to the OLED.
// Does nothing if the framebuffer has not changed.
void oled_update();

// Clear framebuffer. Does not communicate with OLED.
void oled_clear();

// Set every framebuffer pixel on. Does not communicate with OLED.
void oled_fullbright();

// Set/clear one framebuffer pixel. Does not communicate with OLED.
void oled_pixel(uint8_t x, uint8_t y, bool on);

// Print text on specified line (0, 1, 2, 3).Does not communicate with OLED.
void oled_print(const char *text, uint8_t row);

// Format and print the national code. Does not communicate with OLED.
void oled_print_data(FdxbData data, uint8_t row);

// Put OLED controller into display-off/sleep mode.
void oled_sleep();

// Wake OLED after power has been restored. Calls oled_update.
void oled_wake();

#endif
