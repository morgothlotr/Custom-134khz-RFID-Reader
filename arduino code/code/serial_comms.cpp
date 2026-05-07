#include "serial_comms.h"

HardwareSerial Serial1(PA2, PA3);  // RX, TX

void serialInit() {
  Serial1.begin(SERIAL_BAUD);
}

void serialPrint(const char* message) {
  Serial1.print(message);
}

void serialPrintln(const char* message) {
  Serial1.println(message);
}

void serialPrintInt(int value) {
  Serial1.print(value);
}

void serialPrintU64(uint64_t value) {
  Serial1.print((unsigned long long)value);
}
