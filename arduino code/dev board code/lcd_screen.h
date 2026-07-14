/*
 * 134.2 kHz RFID Animal Tag Reader
 * Copyright (C) 2026 University of Canterbury FYP Group E02
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
#ifndef LCD_SCREEN_H
#define LCD_SCREEN_H

#include "fdx_b_decoder.h"

void lcd_init();
void lcd_print_data(FdxbData data);

#endif // LCD_SCREEN_H
