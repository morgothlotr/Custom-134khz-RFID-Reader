#pragma once

#include <Arduino.h>


#define DEMOD_IN PC11

void demodInit();
bool readDemod();