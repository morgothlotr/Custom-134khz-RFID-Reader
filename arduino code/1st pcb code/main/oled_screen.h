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

#define SDA_PIN PB_7
#define SCL_PIN PB_6

#include "fdx_b_decoder.h"

void oled_init();
void oled_print_data(FdxbData data);
void scanning_print();
void scan_failed_print();
void oled_every_pixel_on();

#endif // OLED_SCREEN_H