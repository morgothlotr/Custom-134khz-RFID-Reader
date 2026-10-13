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
#include "serial_comms.h"
#include <HardwareSerial.h>
#include <Arduino.h>

HardwareSerial RfidSerial(SERIAL_RX, SERIAL_TX);  // RX, TX

void serialInit() {
  RfidSerial.begin(SERIAL_BAUD);
}

void serialPrint(const char* message) {
  RfidSerial.print(message);
}

void serialPrintln(const char* message) {
  RfidSerial.println(message);
}

void serialPrintInt(int value) {
  RfidSerial.print(value);
}

void serialPrintNationalCode(uint64_t value) {
  // This is the manual implementation
  if (value == 0) {
    RfidSerial.print("000000000000");
    return;
  }
  uint64_t temp = value;
  int num_digits = 0;
  while (temp > 0) {
    temp /= 10;
    num_digits++;
  }
  int num_zeros = 12 - num_digits;
  for (int i = 0; i < num_zeros; i++) {
    RfidSerial.print('0');
  }
  RfidSerial.print((unsigned long long)value);
}

void serial_print_final_data(FdxbData data) {
  serialPrintln("--- Scan Complete ---");
  serialPrint("National Code: ");
  serialPrintNationalCode(data.nationalCode);
  serialPrintln("");
  serialPrint("Country Code: ");
  serialPrintInt(data.countryCode);
  serialPrintln("");
  serialPrintln("---------------------\n\n");
}
