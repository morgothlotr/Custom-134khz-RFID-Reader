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

// Define a structure to hold the decoded FDX-B data
struct FdxbData {
  uint64_t nationalCode;
  uint16_t countryCode;
  bool animalFlag;
  // Add other fields as needed
};

enum DecoderState {
  SEARCHING_HEADER,
  COLLECTING_BITS,
  PACKET_READY
};

void decoderInit();
bool decoderFeed(int bit);
bool isPacketReady();
FdxbData getDecodedData();
