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

#include "button.h"
#include "pin.h"
#include "buzzer.h"
#include "demod_read.h"
#include "fdx_b_decoder.h"
#include "serial_comms.h"
#include "sleep.h"
//#include "oled_screen.h"

Button button1;
Button button_sleep;

bool awake_time_out = 0;

#define ANTENNA_ALWAYS_ON 0 //1 keeps antenna always on 0 sets normal button operation

// state machine
enum AppState {
  IDLE,
  SCANNING,
  WAKE_START
};
AppState currentState = WAKE_START; //setup makes currentState idle straight away, why have it as WAKE_START?

void setup() {
  pin_init(POWER_LED_ADDRESS);
  pin_init(STATUS_LED_ADDRESS);
  button_init(&button1, BUTTON_1_PIN);
  button_init(&button_sleep, WAKE_PIN);
  buzzer_init();
  serialInit();
  oled_init();
  demod_init();
  decoderInit();
  sleep_init();

  currentState = IDLE;

  serialPrintln("Startup Successful");
}

void loop() {
  //NOTE: 0 is on for LEDS, and 1 is off.
  toggle_pin_update(POWER_LED_ADDRESS, 500);
  set_pin(STATUS_LED_ADDRESS, button_get_state(&button1));
  buzzer_update();
  ButtonEvent button1Event = button_get_event(&button1);
  ButtonEvent button_sleepEvent = button_get_event(&button_sleep);
  if (ANTENNA_ALWAYS_ON) {
    shd_enable(); //for testing
  }
  if (currentState == IDLE) {
    if (button1Event == BUTTON_PRESSED) {
      serialPrintln("BUTTON PUSHED");

      oled_clear();
      oled_print("SCANNING...", 0);
      oled_update();

      buzzer_beep(2000, 200);  // 2 kHz for 200ms
      currentState = SCANNING;
      shd_enable();
      demod_scan_init();  // Reset packet count and start timer
    }
  } else if (currentState == SCANNING) {
    int bit = demod_read_bit();

    if (bit != BIT_NONE) {
      if (decoderFeed(bit)) {
        // A full packet was received, process it
        FdxbData data = getDecodedData();
        ScanStatus status = demod_process_packet(data);

        if (status == SCAN_COMPLETE) {
          // Scan is complete, print the final data
          FdxbData finalData = demod_get_final_data();
          buzzer_beep(2055, 500);  // Success tone
          if (!ANTENNA_ALWAYS_ON) {
            shd_disable();
          }
          // TODO: Process finalData as needed (eeprom)
          serial_print_final_data(finalData);

          oled_clear();
          oled_print("National Code:", 0);
          oled_print_data(finalData, 1);
          oled_update();
          
          currentState = IDLE;
        }
      }
    }

    // Check for scan timeout
    if (demod_is_timed_out()) {
      buzzer_beep(432, 700);  // Timeout tone
      serialPrintln("SCAN TIMED OUT");
      oled_clear();
      oled_print("SCAN TIMED OUT", 0);
      oled_update();

      if (!ANTENNA_ALWAYS_ON) {
        shd_disable();
      }
      currentState = IDLE;
    }
  }
  // TODO: set up sleep mode

  if (button_get_state(&button_sleep)) {
    awake_time_out = 1;
  } else {
    awake_time_out = 0;
  }
    if (awake_time_out) {
    sleep_enter_standby();
  }
}


