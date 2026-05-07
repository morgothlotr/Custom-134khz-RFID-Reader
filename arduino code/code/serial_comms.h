#pragma once

#include <Arduino.h>
#include "fdx_b_decoder.h"

#define SERIAL_BAUD 115200

extern HardwareSerial Serial1;

void serialInit();
void serialPrint(const char* message);
void serialPrintln(const char* message);
void serialPrintInt(int value);
void serial_print_final_data(FdxbData data);