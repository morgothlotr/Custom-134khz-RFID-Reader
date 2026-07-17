#pragma once

#include <Arduino.h>

// Change to the GPIO you have connected to the buzzer
#define BUZZER_PIN PA_1

void buzzer_init();
void buzzer_beep(uint32_t frequency_hz, uint32_t duration_ms);
void buzzer_update();
bool buzzer_is_beeping();