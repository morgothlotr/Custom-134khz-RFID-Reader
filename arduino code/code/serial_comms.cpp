#include "serial_comms.h"

HardwareSerial Serial1(PA2, PA3);  // RX, TX

void serialInit() {
  Serial1.begin(SERIAL_BAUD);
}