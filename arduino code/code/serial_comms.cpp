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

void serialPrintNationalCode(uint64_t value) {
  if (value == 0) {
    Serial1.print("000000000000");
    return;
  }

  // Count the number of digits in the value
  uint64_t temp = value;
  int num_digits = 0;
  while (temp > 0) {
    temp /= 10;
    num_digits++;
  }

  // Print the required number of leading zeros
  int num_zeros = 12 - num_digits;
  for (int i = 0; i < num_zeros; i++) {
    Serial1.print('0');
  }

  // Print the actual number
  Serial1.print((unsigned long long)value);
}
