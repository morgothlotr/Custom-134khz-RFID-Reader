#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Common I2C addresses: 0x27 or 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);  // address, columns, rows

void setup() {
  Wire.begin();        // STM32L4 uses hardware I2C
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("1234568923");
  lcd.setCursor(0, 1);
  lcd.print("I2C LCD Works!");
}

void loop() {
  // your code here
}