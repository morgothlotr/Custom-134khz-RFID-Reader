#include "demod_read.h"
#include "toggle_pin.h"
#include "serial_comms.h"

#define FAST_LOOP_MS 100
#define SLOW_LOOP_MS 1000

unsigned long lastFast = 0;
unsigned long lastSlow = 0;

void setup() {
  serialInit();
  demodInit();
  toggleInit();
}

void loop() {
  unsigned long now = millis();

  if (now - lastFast >= FAST_LOOP_MS) {
    lastFast = now;
    bool demodState = readDemod();
    //Serial1.print("Demod State: ");
    Serial1.println(demodState);
  }

  if (now - lastSlow >= SLOW_LOOP_MS) {
    lastSlow = now;
    togglePin();
  }
}