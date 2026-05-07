#include "demod_read.h"
#include "toggle_pin.h"
#include "serial_comms.h"
#include "fdx_b_decoder.h"

void setup() {
  serialInit();
  demodInit();
  toggleInit();
  decoderInit();
}

void loop() {
  int bit = readDemod();

  if (bit != BIT_NONE) {
    if (decoderFeed(bit)) {
      serialPrintln("\n--- Packet Received ---");
      FdxbData data = getDecodedData();
      serialPrint("National Code: ");
      serialPrintU64(data.nationalCode);
      serialPrintln("");
      serialPrint("Country Code: ");
      serialPrintInt(data.countryCode);
      serialPrintln("");
      serialPrint("Animal Flag: ");
      serialPrintln(data.animalFlag ? "Yes" : "No");
    }
  }
}
