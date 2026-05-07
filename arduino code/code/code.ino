#include "demod_read.h"
#include "toggle_pin.h"
#include "serial_comms.h"

#define SLOW_LOOP_MS 1000

unsigned long lastSlow = 0;

void setup() {
  serialInit();
  demodInit();
  toggleInit();
}

void loop() {
  int pulse = readDemod();

  static int count = 0;

  if (pulse == PULSE_SHORT) {
      serialPrint("S ");
      count++;
  } else if (pulse == PULSE_LONG) {
      serialPrint("L ");
      count++;
  }

  if (count >= 256) {
      serialPrintln("");  // newline every 16 pulses
      count = 0;
  }

}