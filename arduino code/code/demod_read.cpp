#include "demod_read.h"

volatile uint32_t lastEdgeTime = 0;
volatile uint32_t pulseDuration = 0;
volatile bool newPulse = false;

void demodISR() {
  uint32_t now = micros();
  pulseDuration = now - lastEdgeTime;
  lastEdgeTime = now;
  newPulse = true;
}

void demodInit() {
  pinMode(DEMOD_IN, INPUT);
  attachInterrupt(digitalPinToInterrupt(DEMOD_IN), demodISR, CHANGE);
}

int readDemod() {
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
    // Invalid pulse length, reset state
    last_pulse_short = false;
    return BIT_NONE;
  }
}