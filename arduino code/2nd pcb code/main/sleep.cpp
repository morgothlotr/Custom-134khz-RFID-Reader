/*
 * 134.2 kHz RFID Animal Tag Reader
 * Copyright (C) 2026 University of Canterbury FYP Group E02
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

#include "sleep.h"
#include <STM32LowPower.h>
#include "demod_read.h"
#include "oled_screen.h"

#define SLEEP_TIMEOUT_S 20 // Change to 60 after demonstration
static uint32_t last_input_time = 0;

void sleep_init() {
  LowPower.begin();
  LowPower.attachInterruptWakeup(WAKE_PIN, nullptr, RISING, SHUTDOWN_MODE);
  sleep_input();
}

void sleep_input() {
  last_input_time = millis() / 1000;
}

bool sleep_check_timeout() {
  uint16_t current_time_s = millis() / 1000;
  if((current_time_s - last_input_time) > SLEEP_TIMEOUT_S) {
    return true;
  }
  return false;
}

void sleep_enter_standby() {
  // Shutdown tasks
  shd_disable();
  oled_clear();
  oled_update();
  oled_sleep();
  Serial.end();

  // Disable more peripherals
  __HAL_RCC_USART1_CLK_DISABLE();
  __HAL_RCC_USART2_CLK_DISABLE();
  __HAL_RCC_ADC1_CLK_DISABLE();
  __HAL_RCC_SPI1_CLK_DISABLE();
  __HAL_RCC_I2C1_CLK_DISABLE();
  __HAL_RCC_TIM1_CLK_DISABLE();
  __HAL_RCC_TIM2_CLK_DISABLE();
  __HAL_RCC_TIM6_CLK_DISABLE();
  __HAL_RCC_TIM7_CLK_DISABLE();
  __HAL_RCC_RNG_CLK_DISABLE();
  __HAL_RCC_CRC_CLK_DISABLE();
  __HAL_RCC_WWDG_CLK_DISABLE();
  // TODO: need something to ensure writing to storage completes correctly and doesnt stop in the middle

  for(int i = 0; i < NUM_DIGITAL_PINS; i++) {
    if(i != WAKE_PIN) {  // Don't touch the wake pin!
      pinMode(i, INPUT_ANALOG);
    }
  }

  LowPower.shutdown(0);  
}