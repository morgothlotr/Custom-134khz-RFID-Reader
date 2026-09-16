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

extern Twowire OLED_WIRE

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

// Set text cursor in pixel coordinates.
void oled_setCursor(uint8_t x, uint8_t y);

// Print text at current cursor. Does not communicate with OLED.
void oled_print(const char *text);

// Print text and move cursor to next 8-pixel text row.
// Does not communicate with OLED.
void oled_println(const char *text);

// Put OLED controller into display-off/sleep mode.
// Does not turn off your external MOSFET.
void oled_sleep();

// Wake OLED after power has been restored.
// Reinitializes the controller but preserves the framebuffer.
// Call oled_update() afterwards to restore the visible contents.
void oled_wake();

#endif
