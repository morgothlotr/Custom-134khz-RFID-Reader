/*
 * 134.2 kHz RFID Animal Tag Reader
 * Copyright (C) 2026 University of Canterbury FYP Group E02
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "oled_screen.h"

#define OLED_BUFFER_SIZE (OLED_WIDTH * OLED_HEIGHT / 8)

static uint8_t oled_buffer[OLED_BUFFER_SIZE];

static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;
static bool oled_dirty = true;
static bool oled_asleep = false;

Twowire OLED_WIRE(PB9, PB8) // CHANGE TO ACTUAL SDA, SCL PINS

// Low Level I2C functions for sending commands and data
static void oled_command(uint8_t command)
{
    OLED_WIRE.beginTransmission(OLED_ADDR);
    OLED_WIRE.write(0x00);       // command control byte
    OLED_WIRE.write(command);
    OLED_WIRE.endTransmission();
}


static void oled_data(const uint8_t *data, uint8_t length)
{
    OLED_WIRE.beginTransmission(OLED_ADDR);
    OLED_WIRE.write(0x40);       // data control byte

    for (uint8_t i = 0; i < length; i++)
        OLED_WIRE.write(data[i]);

    OLED_WIRE.endTransmission();
}

// Start the controller
static void oled_controller_init()
{
    // Display OFF
    oled_command(0xAE);

    // Display clock
    oled_command(0xD5);
    oled_command(0x80);

    // 128 x 32
    oled_command(0xA8);
    oled_command(0x1F);

    // Display offset
    oled_command(0xD3);
    oled_command(0x00);

    // Start line
    oled_command(0x40);

    // Segment remap
    oled_command(0xA1);

    // COM scan direction
    oled_command(0xC8);

    // COM pin configuration
    oled_command(0xDA);
    oled_command(0x12);

    // Contrast
    oled_command(0x81);
    oled_command(0xFF);

    // Normal display
    oled_command(0xA6);

    // Display follows RAM
    oled_command(0xA4);

    // Horizontal addressing
    oled_command(0x20);
    oled_command(0x00);

    // Visible columns.
    // SSD1305 has 132 RAM columns; this panel uses 128
    oled_command(0x21);
    oled_command(OLED_COL_OFFSET);
    oled_command(127 + OLED_COL_OFFSET);

    // Four pages = 32 rows
    oled_command(0x22);
    oled_command(0x00);
    oled_command(0x03);

    // Charge pump
    oled_command(0x8D);
    oled_command(0x14);

    // Display ON
    oled_command(0xAF);
}

//Start the screen
void oled_init()
{
    OLED_WIRE.begin();

    delay(100);

    oled_controller_init();

    // Blank framebuffer
    for (uint16_t i = 0; i < OLED_BUFFER_SIZE; i++)
        oled_buffer[i] = 0;

    cursor_x = 0;
    cursor_y = 0;

    oled_dirty = true;
    oled_asleep = false;

    // Send initial blank screen
    oled_update();
}


// Have to directly call this function to update the screen, it is not in any other function except init
void oled_update()
{
    // Don't send data while controller is asleep/unpowered
    if (oled_asleep)
        return;

    // Nothing changed.
    if (!oled_dirty)
        return;

    // Horizontal addressing
    oled_command(0x20);
    oled_command(0x00);

    // Column range
    oled_command(0x21);
    oled_command(OLED_COL_OFFSET);
    oled_command(127 + OLED_COL_OFFSET);

    // Page range
    oled_command(0x22);
    oled_command(0x00);
    oled_command(0x03);

    // Send 16 bytes at a time as ram is kinda small
    for (uint16_t i = 0; i < OLED_BUFFER_SIZE; i += 16)
        oled_data(&oled_buffer[i], 16);

    oled_dirty = false;
}



void oled_clear()
{
    for (uint16_t i = 0; i < OLED_BUFFER_SIZE; i++)
        oled_buffer[i] = 0;

    cursor_x = 0;
    cursor_y = 0;

    oled_dirty = true;
}

void oled_fullbright()
{
    for (uint16_t i = 0; i < OLED_BUFFER_SIZE; i++)
        oled_buffer[i] = 0xFF;

    oled_dirty = true;
}

void oled_pixel(uint8_t x, uint8_t y, bool on)
{
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT)
        return;

    uint16_t index = x + ((y / 8) * OLED_WIDTH);
    uint8_t mask = (uint8_t)(1U << (y % 8));

    if (on)
        oled_buffer[index] |= mask;
    else
        oled_buffer[index] &= (uint8_t)~mask;

    oled_dirty = true;
}

// ============================================================
// FONT
// ============================================================
//
// 5x7 font.
// Character width = 5 pixels.
// Character spacing = 1 pixel.
// Text row height = 8 pixels.
//
// Supported:
//   A-Z
//   a-z (rendered as uppercase)
//   0-9
//   _
//   -
//   .
//   ,
//   space
//
// Unsupported characters render as a space.
// ============================================================

static const uint8_t font5x7[][5] =
{
    // SPACE
    {0x00,0x00,0x00,0x00,0x00},

    // ,
    {0x00,0x40,0x20,0x00,0x00},

    // -
    {0x08,0x08,0x08,0x08,0x08},

    // .
    {0x00,0x60,0x60,0x00,0x00},

    // 0
    {0x3E,0x51,0x49,0x45,0x3E},

    // 1
    {0x00,0x42,0x7F,0x40,0x00},

    // 2
    {0x42,0x61,0x51,0x49,0x46},

    // 3
    {0x21,0x41,0x45,0x4B,0x31},

    // 4
    {0x18,0x14,0x12,0x7F,0x10},

    // 5
    {0x27,0x45,0x45,0x45,0x39},

    // 6
    {0x3C,0x4A,0x49,0x49,0x30},

    // 7
    {0x01,0x71,0x09,0x05,0x03},

    // 8
    {0x36,0x49,0x49,0x49,0x36},

    // 9
    {0x06,0x49,0x49,0x29,0x1E},

    // A
    {0x7E,0x11,0x11,0x11,0x7E},

    // B
    {0x7F,0x49,0x49,0x49,0x36},

    // C
    {0x3E,0x41,0x41,0x41,0x22},

    // D
    {0x7F,0x41,0x41,0x22,0x1C},

    // E
    {0x7F,0x49,0x49,0x49,0x41},

    // F
    {0x7F,0x09,0x09,0x09,0x01},

    // G
    {0x3E,0x41,0x49,0x49,0x7A},

    // H
    {0x7F,0x08,0x08,0x08,0x7F},

    // I
    {0x00,0x41,0x7F,0x41,0x00},

    // J
    {0x20,0x40,0x41,0x3F,0x01},

    // K
    {0x7F,0x08,0x14,0x22,0x41},

    // L
    {0x7F,0x40,0x40,0x40,0x40},

    // M
    {0x7F,0x02,0x0C,0x02,0x7F},

    // N
    {0x7F,0x04,0x08,0x10,0x7F},

    // O
    {0x3E,0x41,0x41,0x41,0x3E},

    // P
    {0x7F,0x09,0x09,0x09,0x06},

    // Q
    {0x3E,0x41,0x51,0x21,0x5E},

    // R
    {0x7F,0x09,0x19,0x29,0x46},

    // S
    {0x46,0x49,0x49,0x49,0x31},

    // T
    {0x01,0x01,0x7F,0x01,0x01},

    // U
    {0x3F,0x40,0x40,0x40,0x3F},

    // V
    {0x1F,0x20,0x40,0x20,0x1F},

    // W
    {0x3F,0x40,0x38,0x40,0x3F},

    // X
    {0x63,0x14,0x08,0x14,0x63},

    // Y
    {0x07,0x08,0x70,0x08,0x07},

    // Z
    {0x61,0x51,0x49,0x45,0x43},

    // _
    {0x40,0x40,0x40,0x40,0x40}
};

static int font_index(char c)
{
    if (c == ' ') return 0;
    if (c == ',') return 1;
    if (c == '-') return 2;
    if (c == '.') return 3;

    if (c >= '0' && c <= '9')
        return 4 + (c - '0');

    if (c >= 'A' && c <= 'Z')
        return 14 + (c - 'A');

    if (c >= 'a' && c <= 'z')
        return 14 + (c - 'a');

    if (c == '_')
        return 40;

    return 0;
}

static void oled_char(uint8_t x, uint8_t y, char c)
{
    int index = font_index(c);

    for (uint8_t column = 0; column < 5; column++)
    {
        uint8_t bits = font5x7[index][column];

        for (uint8_t row = 0; row < 7; row++)
        {
            bool on = bits & (1U << row);

            // Avoid modifying pixels outside the display.
            if ((x + column) < OLED_WIDTH &&
                (y + row) < OLED_HEIGHT)
            {
                oled_pixel(x + column, y + row, on);
            }
        }
    }
}

static void oled_write(char c)
{
    if (c == '\n')
    {
        cursor_x = 0;
        cursor_y += 8;

        if (cursor_y + 7 >= OLED_HEIGHT)
            cursor_y = 0;

        return;
    }

    // Wrap before drawing if character won't fit.
    if (cursor_x + 5 >= OLED_WIDTH)
    {
        cursor_x = 0;
        cursor_y += 8;

        if (cursor_y + 7 >= OLED_HEIGHT)
            cursor_y = 0;
    }

    oled_char(cursor_x, cursor_y, c);

    cursor_x += 6;
}

void oled_print(const char *text)
{
    while (*text)
    {
        oled_write(*text);
        text++;
    }

    // Deliberately no oled_update().
}

void oled_println(const char *text)
{
    oled_print(text);

    cursor_x = 0;
    cursor_y += 8;

    if (cursor_y + 7 >= OLED_HEIGHT)
        cursor_y = 0;

    // Deliberately no oled_update().
}

void oled_setCursor(uint8_t x, uint8_t y)
{
    cursor_x = x;
    cursor_y = y;
}

// Call before turning off MOSFET
void oled_sleep()
{
    if (oled_asleep)
        return;

    // Display OFF.
    // This leaves the framebuffer/RAM contents intact.
    oled_command(0xAE);

    oled_asleep = true;
}

// Call after turning on MOSFET
void oled_wake()
{
    if (!oled_asleep)
        return;

    // The external MOSFET must already have restored OLED power
    // before this function is called

    delay(20);

    // Reinitialize the SSD1305 controller
    // This also explicitly restores the correct orientation so the display isn't upside down
    oled_controller_init();

    oled_asleep = false;

    // The controller RAM may have been lost while power was removed
    // Force the framebuffer to be retransmitted
    oled_dirty = true;

    
}
