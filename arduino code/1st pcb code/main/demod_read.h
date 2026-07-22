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
#include "serial_comms.h"
#include "fdx_b_decoder.h" // Needed for FdxbData and ScanStatus

// --- Pulse & Bit Decoding ---
#define DEMOD_IN PB_4
#define NOT_SHD_ENABLE PB_3
#define LVL_SHIFT_ENABLE PA_15

#define FDX_B_SHORT_TIME  128
#define FDX_B_LONG_TIME   256
#define FDX_B_JITTER_TIME 60

#define FDX_B_SHORT_LOW  (FDX_B_SHORT_TIME - FDX_B_JITTER_TIME)
#define FDX_B_SHORT_HIGH (FDX_B_SHORT_TIME + FDX_B_JITTER_TIME)
#define FDX_B_LONG_LOW   (FDX_B_LONG_TIME - FDX_B_JITTER_TIME)
#define FDX_B_LONG_HIGH  (FDX_B_LONG_TIME + FDX_B_JITTER_TIME)

#define PULSE_NONE  -1
#define PULSE_SHORT  0
#define PULSE_LONG   1
#define BIT_NONE    -1

// --- Application-level Scan Logic ---
enum ScanStatus {
  SCAN_IN_PROGRESS,
  SCAN_COMPLETE,
  SCAN_RESET
};

void demod_init();
void print_edges();
void shd_enable();
void shd_disable();
int demod_read_bit();

void demod_scan_init();
ScanStatus demod_process_packet(FdxbData data);
FdxbData demod_get_final_data();
bool demod_is_timed_out();
