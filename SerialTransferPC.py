#!/usr/bin/env python3
"""
serialTransferPC.py

Reads serial data for animal codes from the RFID scanner and logs it into a CSV file.

The PC sends:
    START

The RFID reader responds with:
    <animal code>
    <animal code>
    <animal code>
    END

Usage:
    python3 -m pip install pyserial - if not already installed.
    python serialTransferPC.py
"""

import csv
from datetime import datetime
import time
import serial
import serial.tools.list_ports as list_ports

__author__ = "Blake Manson"
__version__ = "1.0"
__date__ = "2026-09-15"

# ------------------------------
# Configuration
# ------------------------------
OUTPUT_BASENAME = "readings"
BAUDRATE = 115200

# ------------------------------
# Helper functions
# ------------------------------
def get_valid_ports():
    """Return a list of serial ports with valid descriptions."""
    ports = list_ports.comports()
    return [p for p in ports if p.description and p.description.lower() != "n/a"]

def choose_port(ports):
    """Prompt user to select a port from the list."""
    print("Available ports:")
    for i, p in enumerate(ports):
        print(f"{i}: {p.device} - {p.description}")
    choice = int(input("Select port number: "))
    return ports[choice].device

def open_serial(port):
    """Open and return a serial connection."""
    ser = serial.Serial(port, BAUDRATE, timeout=1)
    print(f"Listening on {port} at {BAUDRATE} baud...")
    return ser

def create_timestamped_filename():
    """Create a filename with the current timestamp."""
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    return f"{OUTPUT_BASENAME}_{timestamp}.csv"

# ------------------------------
# Main script
# ------------------------------
def main():

    ports = get_valid_ports()

    if not ports:
        print("No valid serial ports found. Plug in your RFID reader and try again.")
        return

    port = choose_port(ports)
    ser = open_serial(port)
    
    time.sleep(2)

    # Create CSV file
    filename = create_timestamped_filename()
    csvfile = open(filename, "w", newline="")
    writer = csv.writer(csvfile)

    writer.writerow(["Animal Code"])

    # Tell RFID reader to start sending stored data
    ser.write(b"START\n")
    print(f"START sent: Logging to {filename}")

    while True:
        try:
            line = ser.readline().decode(errors="ignore").strip()

            if not line:
                continue

            if line == "END":
                # RFID reader has finished sending data
                print(f"END received: Closed {filename}")

                csvfile.close()
                ser.close()
                break

            else:
                # Each received line is an animal code
                writer.writerow([line])
                csvfile.flush()

                print(f"Animal Code: {line}")

        except KeyboardInterrupt:
            print("\nStopped by user.")
            csvfile.close()
            ser.close()
            break

if __name__ == "__main__":
    main()
