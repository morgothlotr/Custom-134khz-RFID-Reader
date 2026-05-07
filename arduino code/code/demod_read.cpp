#include "demod_read.h"

void demodInit() {
  pinMode(DEMOD_IN, INPUT);
}

bool readDemod() {
  bool demodState = digitalRead(DEMOD_IN);
  return(demodState);
}