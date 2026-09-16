# STM32 Arduino IDE Setup Guide
## Using STM32duino with STM32L476VET6 and ST-Link

---

## Prerequisites

- Windows 10/11 PC
- STM32L476VET6 custom board with on-board ST-Link programmer
- FT232RL USB-to-Serial daughter board (for serial monitor)
- USB cable

---

## Step 1 — Install Arduino IDE 2

1. Download Arduino IDE 2 from [arduino.cc/en/software](https://www.arduino.cc/en/software)
2. Install it (default settings are fine)

> **Note:** Arduino IDE 1.x is no longer supported by the STM32 core. You must use IDE 2.

---

## Step 2 — Add the STM32 Boards URL

1. Open Arduino IDE
2. Go to **File → Preferences**
3. Find the **"Additional Boards Manager URLs"** field
4. Add the following URL:
   ```
   https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json
   ```
5. Click **OK**

---

## Step 3 — Install the STM32 Core

1. Go to **Tools → Board → Boards Manager**
2. Search for `STM32 MCU based boards`
3. Click **Install**

> This is a large download (~500 MB). Allow several minutes to complete.

---

## Step 4 — Install STM32CubeProgrammer

STM32CubeProgrammer is required for flashing over ST-Link. It is separate from CubeIDE — you do **not** need CubeIDE or CubeMX.

1. Download from [st.com/en/development-tools/stm32cubeprog.html](https://www.st.com/en/development-tools/stm32cubeprog.html)
2. Install to a location you have access to, e.g.:
   ```
   C:\apps\STM32CubeProgrammer
   ```
3. During install, accept the ST-Link driver installation when prompted

---

## Step 5 — Add CubeProgrammer to Your PATH

Arduino IDE needs to find `STM32_Programmer_CLI.exe`. You must add the `\bin` folder to your user PATH.

### If you have admin rights:
1. Press **Win + R**, type `sysdm.cpl`, press Enter
2. Click **Advanced → Environment Variables**
3. Under **System variables**, select **Path → Edit**
4. Click **New** and add:
   ```
   C:\apps\STM32CubeProgrammer\bin
   ```
5. Click OK on all dialogs

### If you do NOT have admin rights:
1. Press **Win + R**, paste the following and press Enter:
   ```
   rundll32 sysdm.cpl,EditEnvironmentVariables
   ```
2. In the **top section** (User variables), select **Path → Edit**
3. Click **New** and add:
   ```
   C:\apps\STM32CubeProgrammer\bin
   ```
4. Click OK on all dialogs

### Verify it worked:
Open a **new** Command Prompt window and run:
```
STM32_Programmer_CLI --version
```
You should see output like `STM32CubeProgrammer v2.22.0`. If not, check the path is correct and that you opened a **new** Command Prompt after making the change.

> **Important:** You must fully close and reopen Arduino IDE after updating PATH for it to take effect.

---

## Step 6 — Select Your Board in Arduino IDE

1. Go to **Tools → Board → STM32 MCU based boards → Generic STM32L0 series**
2. Go to **Tools → Board part number** and select:
   ```
   Generic L072KZTx
   ```

---

## Step 7 — Set the Upload Method

1. Go to **Tools → Upload Method**
2. Select **STM32CubeProgrammer (SWD)**

Your on-board ST-Link connects via SWD — no extra wiring needed, just plug in the USB cable.

---

## Step 8 — Set Up Serial Monitor (FT232RL)

The FT232RL daughter board provides a serial connection for `Serial.print` debugging.

### Wiring

| FT232RL Pin | STM32L476 Pin |
|-------------|---------------|
| TX          | PA3 (USART2 RX) |
| RX          | PA2 (USART2 TX) |
| GND         | GND           |

> Do **not** connect 3.3V or 5V from the FT232 to the STM32 — the board is already powered via ST-Link. Only share GND.
> RTS and CTS are not required for basic serial debugging.

### Driver

The FT232RL requires FTDI drivers. These usually install automatically on Windows. If the device does not appear as a COM port in Device Manager, download the driver from [ftdichip.com](https://ftdichip.com/drivers/).

### In Code

Declare the serial port using the correct pins:
```cpp
HardwareSerial Serial1(PA2, PA3);  // RX, TX
```

Initialize in `setup()`:
```cpp
Serial1.begin(115200);
```

### Viewing Output

1. Plug the FT232RL into your PC via USB
2. In Arduino IDE go to **Tools → Port** and select the FT232's COM port (different from the ST-Link port)
3. Open **Tools → Serial Monitor**
4. Set the baud rate to match your `Serial.begin()` value (e.g. 115200)

---

## Step 9 — Upload Your First Sketch

1. Open or write a sketch
2. Click **Upload**
3. A successful flash looks like:
   ```
   Memory Programming ...
   File download complete
   Download verified successfully
   ```

---

## Project File Structure (Recommended)

For anything beyond a simple sketch, use `.h` and `.cpp` files to organise your code:

```
MyProject/
├── MyProject.ino       ← setup() and loop() only
├── serial_comms.h
├── serial_comms.cpp
├── demod_read.h
├── demod_read.cpp
└── ...
```

### Key rules for `.h`/`.cpp` files in Arduino:
- Every `.cpp` file must `#include <Arduino.h>` at the top
- Use `#pragma once` at the top of every `.h` file
- Any variable shared between files must be declared `extern` in the `.h` file
- The return type in the `.h` declaration **must exactly match** the `.cpp` definition

---

## Troubleshooting

| Problem | Fix |
|---|---|
| `STM32_Programmer_CLI not found` | Add `\bin` folder to PATH, restart Arduino IDE |
| `No debug probe detected` in CubeProgrammer | Reinstall ST-Link driver, replug USB |
| ST-Link firmware outdated | Open CubeProgrammer → Help → Firmware Upgrade |
| COM port not appearing for FT232 | Install FTDI driver from ftdichip.com |
| `ambiguating new declaration` error | Return type in `.h` and `.cpp` don't match — fix the `.h` |
| `was not declared in this scope` | Function is missing from the `.h` file — add the declaration |
| `uint64_t` printing incorrectly | Print in two 32-bit halves using `(uint32_t)(val >> 32)` and `(uint32_t)(val)` |

---

## Settings Persistence

Arduino IDE saves board settings **per sketch folder**. To avoid reconfiguring each time:
- Keep a template sketch folder with all Tools settings already configured
- Copy that folder as the starting point for new projects

All IDE preferences are stored at:
```
C:\Users\[username]\AppData\Local\Arduino15\
```

---

## Summary Checklist

- [ ] Arduino IDE 2 installed
- [ ] STM32 boards URL added to Preferences
- [ ] STM32 MCU based boards core installed
- [ ] STM32CubeProgrammer installed
- [ ] CubeProgrammer `\bin` folder added to user PATH
- [ ] New Command Prompt confirms `STM32_Programmer_CLI --version`
- [ ] Arduino IDE reopened after PATH change
- [ ] Board set to **Generic STM32L4 series → Generic L476VETx**
- [ ] Upload method set to **STM32CubeProgrammer (SWD)**
- [ ] FT232RL wired TX→PA3, RX→PA2, GND→GND
- [ ] Correct COM port selected in Tools → Port
- [ ] Baud rate in Serial Monitor matches `Serial.begin()` value
