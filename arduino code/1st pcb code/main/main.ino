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
 
#include "serial_comms.h"
#include "demod_read.h"
#include "fdx_b_decoder.h"
#include "button.h"
#include "toggle_pin.h"
#include "lcd_screen.h"
#include "buzzer.h"

// state machine for reader
enum AppState {
  IDLE,
  SCANNING
};
AppState currentState = IDLE;

void setup() {
  serialInit();
  demod_init();
  decoderInit();
  button_init();
  toggleInit(); // For debug toggle gpio pins
  lcd_init();
  buzzer_init();
}

void loop() {
  buzzer_update();
  if (currentState == IDLE) {
    if (button_is_pressed()) {
      buzzer_beep(2000, 200);
      currentState = SCANNING;
      demod_scan_init(); // This resets packet count AND starts the timer
      scanning_print();
      serialPrintln("Button pressed, starting scan...");
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
          serial_print_final_data(finalData);
          lcd_print_data(finalData);
          currentState = IDLE;
          buzzer_beep(2055, 500);
        }
      }
    }

    // Check for scan timeout
    if (demod_is_timed_out()) {
      buzzer_beep(432, 700);
      scan_failed_print();
      serialPrintln("10 sec scan timed out. No tag found.");
      serialPrintln("Entering IDLE mode.");
      currentState = IDLE;
    }
  }
}
