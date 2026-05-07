#pragma once

#include <Arduino.h>

#define SERIAL_BAUD 115200

extern HardwareSerial Serial1;

void serialInit();
void serialPrint(const char* message);
void serialPrintln(const char* message);
void serialPrintInt(int value);
void serialPrintNationalCode(uint64_t value);