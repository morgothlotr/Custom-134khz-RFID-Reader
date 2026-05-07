#include "demod_read.h"
#include "toggle_pin.h"
#include "serial_comms.h"
#include "fdx_b_decoder.h"

// --- Application State ---
enum AppState {
  IDLE,
  SCANNING
};

AppState currentState = IDLE;

// --- Button Configuration ---
#define BUTTON_PIN PC14 // The pin to read for a button press
#define SIMULATOR_VCC_PIN PC12 // A pin set to HIGH to act as a VCC source for testing

// --- Scan Logic ---
int consecutivePackets = 0;
FdxbData lastValidData;

void setup() {
  serialInit();
  demodInit();
  toggleInit();
  decoderInit();

  // Initialize the button pin with an internal pull-down resistor.
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);

  // Configure the simulator pin as a HIGH output to act as a VCC source
  pinMode(SIMULATOR_VCC_PIN, OUTPUT);
  digitalWrite(SIMULATOR_VCC_PIN, HIGH);
}

// Helper to check if two data packets are identical
bool arePacketsEqual(const FdxbData& d1, const FdxbData& d2) {
    return (d1.nationalCode == d2.nationalCode) && 
           (d1.countryCode == d2.countryCode) &&
           (d1.animalFlag == d2.animalFlag);
}

void loop() {
  // --- Debounce Logic ---
  static unsigned long lastPressTime = 0;
  const unsigned long debounceDelay = 50; // ms

  if (currentState == IDLE) {
    // In IDLE state, we wait for a button press (a HIGH signal).
    if (digitalRead(BUTTON_PIN) == HIGH && (millis() - lastPressTime > debounceDelay)) {
      // Button is pressed, start scanning.
      currentState = SCANNING;
      consecutivePackets = 0;
      serialPrintln("Button pressed, starting scan...");
      lastPressTime = millis(); // Record the time of the press
    }
  } else if (currentState == SCANNING) {
    // In SCANNING state, we process the bitstream.
    int bit = readDemod();

    if (bit != BIT_NONE) {
      if (decoderFeed(bit)) {
        // A full packet has been received from the bitstream
        FdxbData data = getDecodedData();

        // Check if the packet is valid (getDecodedData returns 0 on error)
        if (data.countryCode != 0 || data.nationalCode != 0) {
            serialPrint("Packet found... ");

            if (consecutivePackets == 0) {
                // This is the first valid packet in a potential series.
                lastValidData = data;
                consecutivePackets = 1;
                serialPrintln("(1/3)");
            } else {
                // We have a previous packet, check if this one is identical.
                if (arePacketsEqual(data, lastValidData)) {
                    consecutivePackets++;
                    serialPrint("("); serialPrintInt(consecutivePackets); serialPrintln("/3)");
                } else {
                    // Packets do not match, reset the sequence.
                    serialPrintln("Inconsistent packet, resetting sequence.");
                    lastValidData = data;
                    consecutivePackets = 1;
                }
            }
        } else {
            // An invalid (all-zero) packet was returned, which means a CRC or other error occurred.
            // Reset the count to ensure we only act on consecutive *valid* packets.
            if (consecutivePackets > 0) {
                serialPrintln("Invalid packet received, resetting sequence.");
                consecutivePackets = 0;
            }
        }

        if (consecutivePackets >= 3) {
            // We have 3 consecutive, identical packets. Success!
            serialPrintln("--- Scan Complete ---");
            serialPrint("National Code: ");
            serialPrintU64(data.nationalCode);
            serialPrintln("");
            serialPrint("Country Code: ");
            serialPrintInt(data.countryCode);
            serialPrintln("");

            serialPrintln("Entering IDLE mode.");

            // Return to IDLE state to wait for the next button press.
            currentState = IDLE;
        }
      }
    }
  }
}
