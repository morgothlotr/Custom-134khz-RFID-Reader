#include "serial_comms.h"
#include "demod_read.h"
#include "fdx_b_decoder.h"
#include "button.h"
#include "toggle_pin.h"

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
}

void loop() {
  if (currentState == IDLE) {
    if (button_is_pressed()) {
      currentState = SCANNING;
      demod_scan_init(); // This resets packet count AND starts the timer
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
          currentState = IDLE;
        }
      }
    }

    // Check for scan timeout
    if (demod_is_timed_out()) {
      serialPrintln("10 sec scan timed out. No tag found.");
      serialPrintln("Entering IDLE mode.");
      currentState = IDLE;
    }
  }
}
