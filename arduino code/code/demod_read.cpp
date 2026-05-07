#include "demod_read.h"

// --- Low-level ISR and Bit Decoding Logic ---

volatile uint32_t lastEdgeTime = 0;
volatile uint32_t pulseDuration = 0;
volatile bool newPulse = false;

void demodISR() {
  uint32_t now = micros();
  pulseDuration = now - lastEdgeTime;
  lastEdgeTime = now;
  newPulse = true;
}

void demod_init() {
  pinMode(DEMOD_IN, INPUT);
  attachInterrupt(digitalPinToInterrupt(DEMOD_IN), demodISR, CHANGE);
}

int demod_read_bit() {
  static bool last_pulse_short = false;

  if (!newPulse) {
    return BIT_NONE;
  }

  noInterrupts();
  uint32_t duration = pulseDuration;
  newPulse = false;
  interrupts();

  if (duration >= FDX_B_SHORT_LOW && duration <= FDX_B_SHORT_HIGH) {
    if (last_pulse_short) {
      last_pulse_short = false;
      return 0; // Two short pulses = 0
    } else {
      last_pulse_short = true;
      return BIT_NONE; // First of two short pulses
    }
  } else if (duration >= FDX_B_LONG_LOW && duration <= FDX_B_LONG_HIGH) {
    last_pulse_short = false;
    return 1; // One long pulse = 1
  } else {
    last_pulse_short = false;
    return BIT_NONE;
  }
}


// --- Application-level Scan Logic ---

static int consecutivePackets = 0;
static FdxbData lastValidData;
static unsigned long scanStartTime = 0;
const unsigned long scanTimeout = 10000; // 10 seconds

static bool arePacketsEqual(const FdxbData& d1, const FdxbData& d2) {
    return (d1.nationalCode == d2.nationalCode) && 
           (d1.countryCode == d2.countryCode) &&
           (d1.animalFlag == d2.animalFlag);
}

void demod_scan_init() {
    consecutivePackets = 0;
    scanStartTime = millis();
}

ScanStatus demod_process_packet(FdxbData data) {
    if (data.countryCode != 0 || data.nationalCode != 0) {
        serialPrint("Packet found... ");

        if (consecutivePackets == 0) {
            lastValidData = data;
            consecutivePackets = 1;
            serialPrintln("(1/3)");
            return SCAN_IN_PROGRESS;
        } else {
            if (arePacketsEqual(data, lastValidData)) {
                consecutivePackets++;
                serialPrint("("); serialPrintInt(consecutivePackets); serialPrintln("/3)");
                if (consecutivePackets >= 3) {
                    return SCAN_COMPLETE;
                }
                return SCAN_IN_PROGRESS;
            } else {
                serialPrintln("Inconsistent packet, resetting sequence.");
                lastValidData = data;
                consecutivePackets = 1;
                return SCAN_RESET;
            }
        }
    } else {
        if (consecutivePackets > 0) {
            serialPrintln("Invalid packet received, resetting sequence.");
            demod_scan_init();
            return SCAN_RESET;
        }
    }
    return SCAN_IN_PROGRESS;
}

FdxbData demod_get_final_data() {
    return lastValidData;
}

bool demod_is_timed_out() {
    if (scanStartTime == 0) return false;
    return (millis() - scanStartTime > scanTimeout);
}
