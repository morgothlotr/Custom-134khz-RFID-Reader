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
  if (!newPulse) {
    return PULSE_NONE;
  }

  noInterrupts();
  uint32_t duration = pulseDuration;
  newPulse = false;
  interrupts();

  if (duration >= FDX_B_SHORT_LOW && duration <= FDX_B_SHORT_HIGH) {
    return PULSE_SHORT;  // potential 0 bit
  } else if (duration >= FDX_B_LONG_LOW && duration <= FDX_B_LONG_HIGH) {
    return PULSE_LONG;   // 1 bit
  }

  return PULSE_NONE;  // out of range, ignore
}