/*
 * 134.2 kHz RFID Animal Tag Reader
 * Copyright (C) 2026 University of Canterbury FYP Group E02
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef OLED_SCREEN_H
#define OLED_SCREEN_H

#include <Arduino.h>
#include <Wire.h>
#include "fdx_b_decoder.h"


// ============================================================
// OLED CONFIGURATION
// ============================================================

#define OLED_ADDR        0x3C

#define OLED_WIDTH       128
#define OLED_HEIGHT      32

/*
 * SSD1305 has 132 RAM columns.
 *
 * This particular 128x32 panel uses the visible display
 * starting at RAM column 4.
 */
#define OLED_COL_OFFSET  4


// ============================================================
// I2C PINS
// ============================================================

#define OLED_SDA         PB7
#define OLED_SCL         PB6


// Exposed in case other code needs the bus.
extern TwoWire OLED_WIRE;


// ============================================================
// INITIALISATION / DISPLAY CONTROL
// ============================================================

// Initialise I2C and SSD1305.
void oled_init();

// Send framebuffer to OLED if required.
void oled_update();

// Clear display framebuffer and update OLED.
void oled_clear();

// Set all framebuffer pixels and update OLED.
void oled_fullbright();

// Set or clear one pixel.
// Does not automatically update the OLED.
void oled_pixel(uint8_t x, uint8_t y, bool on);


// ============================================================
// TEXT
// ============================================================

// Print text on row 0-3.
// Text is converted to uppercase.
// Automatically updates OLED.
void oled_print(const char *text, uint8_t row);

// Format and print 12-digit FDX-B national code.
void oled_print_data(FdxbData data, uint8_t row);


// ============================================================
// POWER CONTROL
// ============================================================

// Turn display off.
// Does not cut OLED power.
void oled_sleep();

// Re-initialise and restore display after sleep.
void oled_wake();

#endif
