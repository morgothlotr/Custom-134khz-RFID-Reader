/*
 * Standalone Serial Communication Test
 * For STM32L072 (STM32duino) - custom PCB
 *
 * Tests HardwareSerial on PA_9 (TX) / PA_10 (RX) at 115200 baud.
 *
 * WHAT IT DOES:
 *  1. Prints a counter message once per second (tests TX).
 *  2. Echoes back any character it receives (tests RX + TX together).
 *
 * HOW TO TEST:
 *  - Connect a USB-to-serial adapter (e.g. FTDI) to PA_9/PA_10 on your PCB,
 *    matching TX->RX and RX->TX, plus GND.
 *  - Open a serial terminal (PuTTY, Arduino Serial Monitor, etc.) at 115200 baud.
 *  - You should see the counter incrementing every second.
 *  - Type characters in the terminal - they should be echoed back,
 *    prefixed with "Echo: ".
 */

#define SERIAL_BAUD 115200
#define SERIAL_RX   PA_10
#define SERIAL_TX   PA_9

HardwareSerial Serial1(SERIAL_RX, SERIAL_TX);

unsigned long lastPrint = 0;
unsigned long counter = 0;

void setup() {
  Serial1.begin(SERIAL_BAUD);
  delay(100); // give the UART a moment to stabilize after begin()

  Serial1.println();
  Serial1.println("=== Serial Test Start ===");
  Serial1.print("Baud: ");
  Serial1.println(SERIAL_BAUD);
  Serial1.println("Type something and press enter to test RX echo.");
  Serial1.println("==========================");
}

void loop() {
  // 1. Periodic TX test - proves the UART can send continuously
  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();
    Serial1.print("Heartbeat #");
    Serial1.println(counter++);
  }

  // 2. RX echo test - proves the UART can receive
  if (Serial1.available()) {
    char c = Serial1.read();
    Serial1.print("Echo: ");
    Serial1.println(c);
  }
}