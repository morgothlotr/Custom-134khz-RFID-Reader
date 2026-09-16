/*
 * lcd.c
 *
 *  Created on: Apr 24, 2026
 *      Author: Blake
 */

#include "lcd.h"

extern SPI_HandleTypeDef hspi1;

const uint8_t font5x7[][5] = {
    // Space (32)
    {0x00,0x00,0x00,0x00,0x00},
    // !
    {0x00,0x00,0x5F,0x00,0x00},
    // "
    {0x00,0x07,0x00,0x07,0x00},
    // #
    {0x14,0x7F,0x14,0x7F,0x14},
    // $
    {0x24,0x2A,0x7F,0x2A,0x12},
    // %
    {0x23,0x13,0x08,0x64,0x62},
    // &
    {0x36,0x49,0x55,0x22,0x50},
    // '
    {0x00,0x05,0x03,0x00,0x00},
    // (
    {0x00,0x1C,0x22,0x41,0x00},
    // )
    {0x00,0x41,0x22,0x1C,0x00},
    // *
    {0x14,0x08,0x3E,0x08,0x14},
    // +
    {0x08,0x08,0x3E,0x08,0x08},
    // ,
    {0x00,0x50,0x30,0x00,0x00},
    // -
    {0x08,0x08,0x08,0x08,0x08},
    // .
    {0x00,0x60,0x60,0x00,0x00},
    // /
    {0x20,0x10,0x08,0x04,0x02},

    // 0–9
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9

};

#define LCD_CS_LOW()    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET)
#define LCD_CS_HIGH()   HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET)

#define LCD_A0_LOW()    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET)
#define LCD_A0_HIGH()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET)

#define LCD_RST_LOW()   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET)
#define LCD_RST_HIGH()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET)

void LCD_Init(void)
{
	// 1. Hard Reset (Mandatory)
	    LCD_RST_LOW();
	    HAL_Delay(50);
	    LCD_RST_HIGH();
	    HAL_Delay(50);

	    // 2. LCD Drive Settings
	    LCD_Command(0xA2); // 1/9 Bias
	    LCD_Command(0xA0); // ADC Normal (Column address direction)
	    LCD_Command(0xC8); // COM Output Reverse (Flips the display so text isn't upside down)

	    // 3. Power Control
	    LCD_Command(0x23); // Resistor Ratio (Coarse contrast)
	    LCD_Command(0x2F); // Power Controller ON
	    HAL_Delay(50);  // Allow capacitors to charge up!

	    // 4. Fine Contrast (Electronic Volume)
	    LCD_Command(0x81); // Enter Electronic Volume mode
	    LCD_Command(0x20); // Contrast value (0x00 to 0x3F). Start at mid-point 0x20.

	    // 5. Display On
	    LCD_Command(0xAF);
}

void LCD_Clear(void)
{
    for(uint8_t page = 0; page < 4; page++)
    {
        LCD_Command(0xB0 | page);   // set page
        LCD_Command(0x10);          // column upper
        LCD_Command(0x00);          // column lower

        for(uint8_t col = 0; col < 128; col++)
        {
            LCD_Data(0x00);
        }
    }
}

void LCD_Command(uint8_t cmd)
{
    LCD_A0_LOW();     // command mode
    LCD_CS_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

void LCD_Data(uint8_t data)
{
    LCD_A0_HIGH();    // data mode
    LCD_CS_LOW();
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

void LCD_SetCursor(uint8_t page, uint8_t column)
{
    LCD_Command(0xB0 | page);                 // Set page (0–3)
    LCD_Command(0x10 | (column >> 4));       // Upper column
    LCD_Command(0x00 | (column & 0x0F));     // Lower column
}

void LCD_Char(char c)
{
    if (c < 32 || c > 127)
        c = '?';

    for (uint8_t i = 0; i < 5; i++)
    {
        LCD_Data(font5x7[c - 32][i]);
    }

    LCD_Data(0x00);   // 1 column spacing
}
