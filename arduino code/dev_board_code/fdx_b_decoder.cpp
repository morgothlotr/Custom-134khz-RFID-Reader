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
 * -------------------------------------------------------------------------
 * fdx_b_decoder.cpp
 *
 * This module implementation was developed using public FDX-B protocol
 * information and behavioral reference from the Flipper Zero
 * firmware project. (https://github.com/flipperdevices/flipperzero-firmware/lib/lfrfid/protocols/protocol_fdx_b.c)
 *
 * It is an independently written implementation and is not a
 * verbatim copy of the original source code.
 */

#include "fdx_b_decoder.h"
#include "serial_comms.h"

#define PACKET_BIT_SIZE 128
#define HEADER_BIT_SIZE 11
#define DATA_BIT_SIZE 88 // 117 bits total data/control - 13 control bits removed = 104 data bits.
#define CLEAN_DATA_BYTE_SIZE (DATA_BIT_SIZE / 8)

// The FDX-B header is 10000000000
const uint16_t FDX_B_HEADER = 0b10000000000;

struct DecoderInternalState {
  DecoderState state;
  uint8_t packetBuffer[PACKET_BIT_SIZE / 8];
  uint16_t bitCount;
  uint16_t window; // For header searching
};

static DecoderInternalState s_decoder;

// Standard CRC-16 CCITT implementation
static uint16_t crc16_ccitt(const uint8_t* data, size_t length) {
    uint16_t crc = 0x0000;
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

void decoderInit() {
  s_decoder.state = SEARCHING_HEADER;
  s_decoder.bitCount = 0;
  s_decoder.window = 0;
  memset(s_decoder.packetBuffer, 0, sizeof(s_decoder.packetBuffer));
}

bool decoderFeed(int bit) {
  if (s_decoder.state == PACKET_READY) {
    return false;
  }

  s_decoder.window = ((s_decoder.window << 1) & 0x7FF) | (bit & 1);

  if (s_decoder.state == SEARCHING_HEADER) {
    if (s_decoder.window == FDX_B_HEADER) {
      s_decoder.state = COLLECTING_BITS;
      s_decoder.bitCount = HEADER_BIT_SIZE;
      memset(s_decoder.packetBuffer, 0, sizeof(s_decoder.packetBuffer));
      // Store the header correctly from the window
      for(int i = 0; i < HEADER_BIT_SIZE; i++) {
        int bit = (s_decoder.window >> (HEADER_BIT_SIZE - 1 - i)) & 1;
        if (bit) {
            int byteIndex = i / 8;
            int bitIndex = 7 - (i % 8);
            s_decoder.packetBuffer[byteIndex] |= (1 << bitIndex);
        }
      }
    }
  } else if (s_decoder.state == COLLECTING_BITS) {
    int byteIndex = s_decoder.bitCount / 8;
    int bitIndex = 7 - (s_decoder.bitCount % 8);
    if (bit) {
      s_decoder.packetBuffer[byteIndex] |= (1 << bitIndex);
    }
    s_decoder.bitCount++;

    if (s_decoder.bitCount >= PACKET_BIT_SIZE) {
      s_decoder.state = PACKET_READY;
      return true; 
    }
  }
  return false;
}

bool isPacketReady() {
  return s_decoder.state == PACKET_READY;
}

// Helper function to extract a sequence of bits from a buffer
static uint64_t extract_bits(const uint8_t* buffer, int start_bit, int num_bits) {
    uint64_t result = 0;
    for (int i = 0; i < num_bits; i++) {
        int bit_addr = start_bit + i;
        int byte_index = bit_addr / 8;
        int bit_index = 7 - (bit_addr % 8);
        if ((buffer[byte_index] >> bit_index) & 1) {
            result |= 1ULL << i;
        }
    }
    return result;
}

FdxbData getDecodedData() {
  FdxbData data = {0, 0, false};
  uint8_t cleaned_data[CLEAN_DATA_BYTE_SIZE] = {0};
  int clean_bit_count = 0;

  // 1. Validate control bits and extract clean data
  for (int i = 0; i < 13; i++) {
    int start_of_chunk = HEADER_BIT_SIZE + i * 9;
    // Check control bit (the FIRST bit of the 9-bit chunk)
    int control_bit_addr = start_of_chunk;
    int byte_index = control_bit_addr / 8;
    int bit_index = 7 - (control_bit_addr % 8);
    if (!((s_decoder.packetBuffer[byte_index] >> bit_index) & 1)) {
       serialPrintln("Control bit error");
       decoderInit();
       return data;
    }
    // Copy data bits (the next 8 bits)
    for (int j = 0; j < 8; j++) {
        int bit_addr = start_of_chunk + 1 + j;
        byte_index = bit_addr / 8;
        bit_index = 7 - (bit_addr % 8);
        int bit = (s_decoder.packetBuffer[byte_index] >> bit_index) & 1;
        if (bit) {
            int clean_byte_index = clean_bit_count / 8;
            int clean_bit_index = 7 - (clean_bit_count % 8);
            cleaned_data[clean_byte_index] |= (1 << clean_bit_index);
        }
        clean_bit_count++;
    }
  }

  // 2. CRC Check

  uint16_t received_crc = (cleaned_data[8] << 8) | cleaned_data[9];
  uint16_t calculated_crc = crc16_ccitt(cleaned_data, 8);

  if (calculated_crc != received_crc) {
    serialPrintln("CRC error");
    serialPrint("Expected: "); serialPrintInt(received_crc);
    serialPrint(" Got: "); serialPrintInt(calculated_crc);
    serialPrintln("");
    decoderInit();
    return data;
  }

  // 3. Extract data using bit-level functions

  
  uint64_t raw_national_code = extract_bits(cleaned_data, 0, 38);
  data.nationalCode = raw_national_code;

  uint64_t raw_country_code = extract_bits(cleaned_data, 38, 10);
  data.countryCode = raw_country_code;
  
  data.animalFlag = extract_bits(cleaned_data, 48, 1);

  decoderInit();
  return data;
}