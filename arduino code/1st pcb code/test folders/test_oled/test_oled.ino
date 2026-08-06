#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

TwoWire Oled(PB6, PB7);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Oled, -1);

void setup()
{
    Oled.begin();

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        while (1);
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("Hello");
    display.display();
}

void loop()
{
    
}