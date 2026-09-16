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

Button button1;

// state machine
enum AppState {
  IDLE,
  SCANNING,
  WAKE_START
};
AppState currentState = WAKE_START;

void setup() {
  pin_init(POWER_LED_ADDRESS);
  pin_init(STATUS_LED_ADDRESS);
  button_init(&button1, BUTTON_1_PIN);
  buzzer_init();
  serialInit();

  demod_init();
  decoderInit();

  currentState = IDLE;

  //NOTE: 0 is on for LEDS, and 1 is off. Button appears to be correct
  serialPrintln("Startup Successful");
}

void loop() {
  toggle_pin_update(POWER_LED_ADDRESS, 500);
  set_pin(STATUS_LED_ADDRESS, button_get_state(&button1));
  buzzer_update();

  ButtonEvent buttonEvent = button_get_event(&button1);
  if (currentState == IDLE) {
    if (buttonEvent == BUTTON_PRESSED) {
      serialPrintln(BUTTON PUSHED);
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
          shd_disable();
          // TODO: Process finalData as needed (display, eeprom)
          serial_print_final_data();
          
          currentState = IDLE;
        }
      }
    }

    // Check for scan timeout
    if (demod_is_timed_out()) {
      buzzer_beep(432, 700);  // Timeout tone
      // TODO: Handle timeout (print error message)
      serialPrintln(TIMING OUT);
      shd_disable();
      currentState = IDLE;
    }
  }
  // TODO: set up sleep mode
}

