/*
 * Standalone LCD Test - Button Press Counter
 * For STM32L072 (STM32duino) - custom PCB
 *
 * Set SDA_PIN / SCL_PIN / BUTTON_PIN below to your actual PCB pins.
 * Button is ACTIVE HIGH (external pull-down on board per schematic).
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- SET THESE TO YOUR ACTUAL PINS ---
#define SDA_PIN PB_7
#define SCL_PIN PB_6
#define BUTTON_PIN PA_2
// --------------------------------------

#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
#define DEBOUNCE_MS 50

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

int pressCount = 0;
bool lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
bool countedThisPress = false;

void updateDisplay() {
  lcd.setCursor(0, 0);
  lcd.print("Button Presses:");
  lcd.setCursor(0, 1);
  lcd.print(pressCount);
  lcd.print("   "); // pad to clear leftover digits
}

void setup() {
  pinMode(BUTTON_PIN, INPUT); // external pull-down already on board

  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin();

  lcd.init();
  lcd.backlight();
  lcd.clear();

  updateDisplay();
}

void loop() {
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_MS) {
    if (reading == HIGH && !countedThisPress) {
      pressCount++;
      updateDisplay();
      countedThisPress = true;
    } else if (reading == LOW) {
      countedThisPress = false;
    }
  }

  lastButtonState = reading;
}