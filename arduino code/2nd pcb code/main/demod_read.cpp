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
#include "demod_read.h"

// --- Low-level ISR and Bit Decoding Logic ---

volatile uint32_t lastEdgeTime = 0;
volatile uint32_t pulseDuration = 0;
volatile bool newPulse = false;
volatile uint32_t edges = 0;

void demodISR() {
  edges++;
  uint32_t now = micros();
  pulseDuration = now - lastEdgeTime;
  lastEdgeTime = now;
  newPulse = true;
}


void print_edges() {
  #ifdef SERIAL_COMMS_H
    serialPrintInt(edges);
  #endif
}

void demod_init() {
  pinMode(DEMOD_IN, INPUT);
  attachInterrupt(digitalPinToInterrupt(DEMOD_IN), demodISR, CHANGE);
  
  shd_disable();
}

void shd_enable() {
  pinMode(NOT_SHD_ENABLE, OUTPUT);
  digitalWrite(NOT_SHD_ENABLE, LOW);;
}

void shd_disable() {
  pinMode(NOT_SHD_ENABLE, INPUT);
}

int demod_read_bit() {
  static bool last_pulse_short = false;

  if (!newPulse) {
    return BIT_NONE;
  }

  noInterrupts();
  uint32_t duration = pulseDuration;
  newPulse = false;
  interrupts();

  if (duration >= FDX_B_SHORT_LOW && duration <= FDX_B_SHORT_HIGH) {
    if (last_pulse_short) {
      last_pulse_short = false;
      return 0; // Two short pulses = 0
    } else {
      last_pulse_short = true;
      return BIT_NONE; // First of two short pulses
    }
  } else if (duration >= FDX_B_LONG_LOW && duration <= FDX_B_LONG_HIGH) {
    last_pulse_short = false;
    return 1; // One long pulse = 1
  } else {
    last_pulse_short = false;
    return BIT_NONE;
  }
}


// --- Application-level Scan Logic ---
static FdxbData lastValidData;
static unsigned long scanStartTime = 0;
const unsigned long scanTimeout = 10000; // 10 seconds

void demod_scan_init() {
  scanStartTime = millis();
}

ScanStatus demod_process_packet(FdxbData data) {
  if (data.countryCode != 0 || data.nationalCode != 0) {
    #ifdef SERIAL_COMMS_H
      serialPrint("Packet found... ");
    #endif
    lastValidData = data;
    return SCAN_COMPLETE;
  }
  return SCAN_IN_PROGRESS;
}

FdxbData demod_get_final_data() {
  return lastValidData;
}

bool demod_is_timed_out() {
  if (scanStartTime == 0) return false;
  return (millis() - scanStartTime > scanTimeout);  
}
