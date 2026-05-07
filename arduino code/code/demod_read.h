#pragma once

#include <Arduino.h>
#include "serial_comms.h"
#include "fdx_b_decoder.h" // Needed for FdxbData and ScanStatus

// --- Pulse & Bit Decoding ---
#define DEMOD_IN PC11

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
int demod_read_bit();

void demod_scan_init();
ScanStatus demod_process_packet(FdxbData data);
FdxbData demod_get_final_data();
bool demod_is_timed_out();
