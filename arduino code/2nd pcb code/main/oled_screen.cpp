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
#define OLED_I2C_SPEED   50000UL
#define OLED_PAGE_COUNT  4
#define OLED_RAM_WIDTH   132

TwoWire OLED_WIRE(OLED_SDA, OLED_SCL);

static uint8_t oled_buffer[OLED_BUFFER_SIZE];

static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

static bool oled_dirty = true;
static bool oled_asleep = false;

// LOW LEVEL I2C
static bool oled_command(uint8_t command)
{
    OLED_WIRE.beginTransmission(OLED_ADDR);
    OLED_WIRE.write(0x00); // SSD1305 command control byte
    OLED_WIRE.write(command);
    return OLED_WIRE.endTransmission() == 0;
}


static bool oled_data(const uint8_t *data, uint8_t length)
{
    OLED_WIRE.beginTransmission(OLED_ADDR);
    OLED_WIRE.write(0x40); // SSD1305 data control byte
    for (uint8_t i = 0; i < length; i++)
    {
        OLED_WIRE.write(data[i]);
    }
    return OLED_WIRE.endTransmission() == 0;
}


// ============================================================
// SET PAGE / COLUMN
// ============================================================
//
// The SSD1305 has 132 RAM columns.
//
// Our panel exposes 128 pixels beginning at RAM column 4.
//
// Therefore:
//     framebuffer X = 0
//     SSD1305 RAM X = 4
//
// Page addressing mode is used because it was verified to
// work correctly with this panel.
// ============================================================

static void oled_set_page(uint8_t page)
{
    if (page >= OLED_PAGE_COUNT)
        return;

    // Select page
    oled_command(0xB0 | page);

    // Set RAM column = OLED_COL_OFFSET
    //
    // Low nibble
    oled_command(0x00 | (OLED_COL_OFFSET & 0x0F));

    // High nibble
    oled_command(0x10 | ((OLED_COL_OFFSET >> 4) & 0x0F));
}


// ============================================================
// SSD1305 CONTROLLER INITIALISATION
// ============================================================

static void oled_controller_init()
{
    oled_command(0xAE); // Display OFF

    oled_command(0xD5);
    oled_command(0x80); // Display Clock

    oled_command(0xA8); // Multiplex Ratio
    oled_command(0x1F);

    oled_command(0xD3); // Display Offset
    oled_command(0x00);

    oled_command(0x40); // Display Start Line

    oled_command(0xA1); // Segment Remap

    oled_command(0xC8); // COM Scan Direction

    oled_command(0xDA); // COM Hardware Configuration
    oled_command(0x12);

    oled_command(0x81); // Contrast
    oled_command(0x40);

    oled_command(0xA6); // Normal Display

    oled_command(0xA4); // Display Follows RAM

    oled_command(0x8D); // Charge Pump
    oled_command(0x14);

    oled_command(0xD9); // Pre-charge Period
    oled_command(0xF1);

    oled_command(0xDB); // VCOMH deselect level
    oled_command(0x40);

    oled_command(0x20); // Page Addressing Mode
    oled_command(0x02);

    delay(100);
    oled_command(0xAF); // Display On
    delay(100);
}


// ============================================================
// CLEAR CONTROLLER RAM
// ============================================================
//
// This is deliberately done during initialization.
//
// We found during testing that stale/random controller RAM
// contents could remain visible after power interruptions.
//
// The SSD1305 has 132 columns, so clear all 132 columns,
// not just the 128 visible columns.
// ============================================================

static void oled_clear_controller_ram()
{
    uint8_t zeros[16];

    for (uint8_t i = 0; i < sizeof(zeros); i++)
    {
        zeros[i] = 0x00;
    }


    for (uint8_t page = 0; page < OLED_PAGE_COUNT; page++)
    {
        // Select page
        oled_command(0xB0 | page);

        // Start at RAM column 0.
        //
        // We deliberately start at zero here because we want
        // to clear ALL 132 RAM columns.
        oled_command(0x00);
        oled_command(0x10);


        for (uint8_t column = 0;
             column < OLED_RAM_WIDTH;
             column += sizeof(zeros))
        {
            uint8_t count = sizeof(zeros);

            if ((column + count) > OLED_RAM_WIDTH)
            {
                count = OLED_RAM_WIDTH - column;
            }

            oled_data(zeros, count);
        }
    }
}


// ============================================================
// OLED INITIALISATION
// ============================================================

void oled_init()
{
    // Start I2C
    OLED_WIRE.begin();

    // Keep the conservative speed that worked during testing.
    OLED_WIRE.setClock(OLED_I2C_SPEED);

    delay(200);


    // Initialise controller
    oled_controller_init();


    // Clear MCU framebuffer
    for (uint16_t i = 0; i < OLED_BUFFER_SIZE; i++)
    {
        oled_buffer[i] = 0x00;
    }


    // Clear actual SSD1305 RAM
    oled_clear_controller_ram();


    // Reset software state
    cursor_x = 0;
    cursor_y = 0;

    oled_dirty = true;
    oled_asleep = false;


    // Send blank framebuffer
    oled_update();
}


// ============================================================
// UPDATE DISPLAY
// ============================================================

void oled_update()
{
    if (oled_asleep)
        return;

    if (!oled_dirty)
        return;


    uint8_t block[16];


    // Four pages for 32-pixel-high display.
    for (uint8_t page = 0; page < OLED_PAGE_COUNT; page++)
    {
        oled_set_page(page);


        uint16_t base = page * OLED_WIDTH;


        // Send 128 visible columns.
        for (uint8_t x = 0; x < OLED_WIDTH; x += 16)
        {
            for (uint8_t i = 0; i < 16; i++)
            {
                block[i] = oled_buffer[base + x + i];
            }

            oled_data(block, 16);
        }
    }


    oled_dirty = false;
}


// ============================================================
// CLEAR FRAMEBUFFER
// ============================================================

void oled_clear()
{
    for (uint16_t i = 0; i < OLED_BUFFER_SIZE; i++)
    {
        oled_buffer[i] = 0x00;
    }


    cursor_x = 0;
    cursor_y = 0;

    oled_dirty = true;
}


// ============================================================
// FULL BRIGHT
// ============================================================

void oled_fullbright()
{
    for (uint16_t i = 0; i < OLED_BUFFER_SIZE; i++)
    {
        oled_buffer[i] = 0xFF;
    }

    oled_dirty = true;
}


// ============================================================
// PIXEL
// ============================================================

void oled_pixel(uint8_t x, uint8_t y, bool on)
{
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT)
        return;


    uint16_t index =
        x + ((y / 8) * OLED_WIDTH);


    uint8_t mask =
        (uint8_t)(1U << (y % 8));


    if (on)
    {
        oled_buffer[index] |= mask;
    }
    else
    {
        oled_buffer[index] &= (uint8_t)~mask;
    }


    oled_dirty = true;
}


// ============================================================
// FONT
// ============================================================
//
// 5x7 font
//
// Character width = 5 pixels
// Character spacing = 1 pixel
// Character height = 7 pixels
// Row spacing = 8 pixels
//
// Supported:
//     A-Z
//     a-z
//     0-9
//     _
//     -
//     .
//     ,
//     space
//
// Lowercase characters are rendered as uppercase.
// Unsupported characters become spaces.
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


// ============================================================
// FONT INDEX
// ============================================================

static int font_index(char c)
{
    if (c == ' ')
        return 0;

    if (c == ',')
        return 1;

    if (c == '-')
        return 2;

    if (c == '.')
        return 3;


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


// ============================================================
// DRAW CHARACTER
// ============================================================

static void oled_char(uint8_t x, uint8_t y, char c)
{
    int index = font_index(c);


    for (uint8_t column = 0; column < 5; column++)
    {
        uint8_t bits = font5x7[index][column];


        for (uint8_t row = 0; row < 7; row++)
        {
            if (bits & (1U << row))
            {
                uint8_t px = x + column;
                uint8_t py = y + row;


                if (px < OLED_WIDTH &&
                    py < OLED_HEIGHT)
                {
                    oled_pixel(px, py, true);
                }
            }
        }
    }
}


// ============================================================
// WRITE CHARACTER
// ============================================================

static void oled_write(char c)
{
    if (c == '\n')
    {
        cursor_x = 0;
        cursor_y += 8;


        if (cursor_y + 7 >= OLED_HEIGHT)
        {
            cursor_y = 0;
        }

        return;
    }


    // Character is 5 pixels wide.
    // One pixel of spacing is added.
    if (cursor_x + 5 >= OLED_WIDTH)
    {
        cursor_x = 0;
        cursor_y += 8;


        if (cursor_y + 7 >= OLED_HEIGHT)
        {
            cursor_y = 0;
        }
    }


    oled_char(cursor_x, cursor_y, c);

    cursor_x += 6;
}


// ============================================================
// CLEAR ONE TEXT ROW
// ============================================================

static void oled_clear_row(uint8_t row)
{
    if (row >= 4)
        return;


    uint8_t y_start = row * 8;


    for (uint8_t y = y_start;
         y < y_start + 8;
         y++)
    {
        for (uint8_t x = 0;
             x < OLED_WIDTH;
             x++)
        {
            oled_pixel(x, y, false);
        }
    }
}


// ============================================================
// PRINT TEXT
// ============================================================

void oled_print(const char *text, uint8_t row)
{
    if (row >= 4)
        return;


    // Clear only this row first.
    oled_clear_row(row);


    cursor_x = 0;
    cursor_y = row * 8;


    while (*text)
    {
        oled_write(*text);
        text++;
    }


    // Explicitly send framebuffer to SSD1305.
    oled_update();
}


// ============================================================
// PRINT FDX-B NATIONAL CODE
// ============================================================

void oled_print_data(FdxbData data, uint8_t row)
{
    char buf[13];
    uint64_t temp = data.nationalCode;

    for (int i = 11; i >= 0; i--)
    {
        buf[i] = (char)((temp % 10) + '0');
        temp /= 10;
    }

    buf[12] = '\0';
    oled_print(buf, row);
}


// ============================================================
// OLED SLEEP
// ============================================================

void oled_sleep()
{
    if (oled_asleep)
        return;


    oled_command(0xAE);

    oled_asleep = true;
}


// ============================================================
// OLED WAKE
// ============================================================

void oled_wake()
{
    if (!oled_asleep)
        return;


    delay(20);


    // Reinitialise controller.
    oled_controller_init();


    // Clear controller RAM because power cycling can leave
    // stale contents behind.
    oled_clear_controller_ram();


    oled_asleep = false;

    oled_dirty = true;


    // Restore framebuffer.
    oled_update();
}
