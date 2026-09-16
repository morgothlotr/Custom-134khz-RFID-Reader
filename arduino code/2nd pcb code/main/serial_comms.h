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
#include "fdx_b_decoder.h"

#define SERIAL_BAUD 115200
#define SERIAL_RX PA_10
#define SERIAL_TX PA_9

extern HardwareSerial& RfidSerial;

void serialInit();
void serialPrint(const char* message);
void serialPrintln(const char* message);
void serialPrintInt(int value);
void serial_print_final_data(FdxbData data);