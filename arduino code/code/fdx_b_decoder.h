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
