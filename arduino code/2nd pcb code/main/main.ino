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
//#include "oled_screen.h"

Button button1;

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
  buzzer_init();
  serialInit();
  //oled_init();
  demod_init();
  decoderInit();

  currentState = IDLE;

  serialPrintln("Startup Successful");
}

void loop() {
  //Remove one devie works
  //NOTE: 0 is on for LEDS, and 1 is off.
  toggle_pin_update(POWER_LED_ADDRESS, 500);
  set_pin(STATUS_LED_ADDRESS, button_get_state(&button1));


  buzzer_update();
  ButtonEvent buttonEvent = button_get_event(&button1);

  if (currentState == IDLE) {
    if (buttonEvent == BUTTON_PRESSED) {
      serialPrintln("BUTTON PUSHED");
      //oled_clear();
      //oled_print("SCANNING...", 0);
      //oled_update();
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
          // TODO: Process finalData as needed (eeprom)
          serial_print_final_data(finalData);
          //oled_clear();
          //oled_print("National Code:", 0);
          //oled_print_data(finalData, 1);
          //oled_update();
          
          currentState = IDLE;
        }
      }
    }

    // Check for scan timeout
    if (demod_is_timed_out()) {
      buzzer_beep(432, 700);  // Timeout tone
      serialPrintln("SCAN TIMED OUT");
      //oled_clear();
      //oled_print("SCAN TIMED OUT", 0);
      //oled_update();

      shd_disable();
      currentState = IDLE;
    }
  }
  // TODO: set up sleep mode
  // Probably need to make the button an interrupt not a loop but not sure yet
}


