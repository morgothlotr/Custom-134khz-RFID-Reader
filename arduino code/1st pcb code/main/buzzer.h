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

#define BUZZER_PIN PA_1

struct BeepStep {
    uint16_t frequency_hz;
    uint16_t duration_ms;
    uint16_t gap_ms;
};

extern const BeepStep successBeep[];
extern const uint8_t successBeepLength;

extern const BeepStep errorBeep[];
extern const uint8_t errorBeepLength;

void buzzer_init();

void buzzer_play(const BeepStep *sequence, uint8_t length);

void buzzer_update();

bool buzzer_is_busy();