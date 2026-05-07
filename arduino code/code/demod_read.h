#pragma once

#include <Arduino.h>
#include "serial_comms.h"

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

void demodInit();
int readDemod();