/*
 * 134.2 kHz RFID Animal Tag Reader
 * Copyright (C) 2026 University of Canterbury FYP Group E02
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
#include "lcd_screen.h"
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define READY_TO_SCAN "Ready to scan"
// I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);  // address, columns, rows

void lcd_init() {
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin();        // STM32L4 uses hardware I2C
  lcd.init();
  lcd.backlight();     // Turn on backlight immediately
  lcd.clear();         // Clear screen
  lcd.setCursor(0, 0);
  lcd.print(READY_TO_SCAN);
}

void scanning_print() {
  lcd.clear();         // Clear screen
  lcd.setCursor(0, 0);
  lcd.print("Scanning...");
}

void scan_failed_print() {
  lcd.clear();         // Clear screen
  lcd.setCursor(0, 0);
  lcd.print(READY_TO_SCAN);
  lcd.setCursor(0, 1);
  lcd.print("Scan timed out");
}

void lcd_print_data(FdxbData data) {
  lcd.clear(); // Clear old data before printing
  lcd.setCursor(0, 0);
  
  // Format the 12-digit national code with leading zeros into a char buffer
  // This avoids any errors from lcd module not wanting to print uint64_t / unsigned long long
  if (data.nationalCode == 0) {
    lcd.print("000000000000");
  } else {
    char buf[13];
    uint64_t temp = data.nationalCode;
    for (int i = 11; i >= 0; i--) {
      buf[i] = (char)((temp % 10) + '0');
      temp /= 10;
    }
    buf[12] = '\0'; // Null-terminate the string
    lcd.print(buf);
  }

  //lcd.setCursor(0, 1);
  //lcd.print("Country: ");
  //lcd.print(data.countryCode);
}
