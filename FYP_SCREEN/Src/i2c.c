/*
 * i2c.c
 *
 *  Created on: May 8, 2026
 *      Author: Blake
 */

#include "i2c.h"

#define LCD_ADDR      (0x27 << 1)   // Try (0x3F << 1) if 0x27 does not work
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_RS        0x01

static I2C_HandleTypeDef *lcd_i2c;

static void lcd_send_internal(uint8_t data, uint8_t flags)
{
    uint8_t upper = data & 0xF0;
    uint8_t lower = (data << 4) & 0xF0;

    uint8_t buffer[4];

    buffer[0] = upper | flags | LCD_BACKLIGHT | LCD_ENABLE;
    buffer[1] = upper | flags | LCD_BACKLIGHT;
    buffer[2] = lower | flags | LCD_BACKLIGHT | LCD_ENABLE;
    buffer[3] = lower | flags | LCD_BACKLIGHT;

    HAL_I2C_Master_Transmit(lcd_i2c, LCD_ADDR, buffer, 4, HAL_MAX_DELAY);
}

static void lcd_send_cmd(uint8_t cmd)
{
    lcd_send_internal(cmd, 0x00);
}

static void lcd_send_data(uint8_t data)
{
    lcd_send_internal(data, LCD_RS);
}

void lcd_init(I2C_HandleTypeDef *hi2c)
{
    lcd_i2c = hi2c;

    HAL_Delay(50);

    lcd_send_cmd(0x30);
    HAL_Delay(5);
    lcd_send_cmd(0x30);
    HAL_Delay(1);
    lcd_send_cmd(0x30);
    HAL_Delay(10);

    lcd_send_cmd(0x20); // 4-bit mode
    HAL_Delay(10);

    lcd_send_cmd(0x28); // 2 lines, 5x8 font
    HAL_Delay(1);

    lcd_send_cmd(0x08); // display off
    HAL_Delay(1);

    lcd_clear();

    lcd_send_cmd(0x06); // increment cursor
    HAL_Delay(1);

    lcd_send_cmd(0x0C); // display on, cursor off
}

void lcd_clear(void)
{
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_put_cur(uint8_t row, uint8_t col)
{
    uint8_t address;

    if (row == 0)
        address = 0x80 + col;
    else
        address = 0xC0 + col;

    lcd_send_cmd(address);
}

void lcd_send_string(char *str)
{
    while (*str)
    {
        lcd_send_data((uint8_t)(*str));
        str++;
    }
}
