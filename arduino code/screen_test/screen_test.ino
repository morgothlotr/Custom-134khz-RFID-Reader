#include <Arduino.h>
#include <Wire.h>

#define OLED_ADDR        0x3C

#define OLED_WIDTH       128
#define OLED_HEIGHT      32
#define OLED_COL_OFFSET  4

#define OLED_SDA         PB7
#define OLED_SCL         PB6

#define STATUS_LED       PB1

TwoWire OLED_WIRE(OLED_SDA, OLED_SCL);


// ============================================================
// LOW LEVEL I2C
// ============================================================

static void oled_command(uint8_t command)
{
    OLED_WIRE.beginTransmission(OLED_ADDR);
    OLED_WIRE.write(0x00);
    OLED_WIRE.write(command);
    OLED_WIRE.endTransmission();
}


static void oled_data(const uint8_t *data, uint8_t length)
{
    OLED_WIRE.beginTransmission(OLED_ADDR);

    OLED_WIRE.write(0x40);

    for (uint8_t i = 0; i < length; i++)
        OLED_WIRE.write(data[i]);

    OLED_WIRE.endTransmission();
}


// ============================================================
// SSD1305 INITIALISATION
// ============================================================

static void oled_controller_init()
{
    oled_command(0xAE);       // Display OFF

    oled_command(0xD5);       // Display clock
    oled_command(0x80);

    oled_command(0xA8);       // Multiplex
    oled_command(0x1F);       // 32 rows

    oled_command(0xD3);       // Display offset
    oled_command(0x00);

    oled_command(0x40);       // Start line 0

    oled_command(0xA1);       // Segment remap
    oled_command(0xC8);       // COM scan direction

    // Your panel requires this
    oled_command(0xDA);
    oled_command(0x12);

    oled_command(0x81);       // Contrast
    oled_command(0x40);

    oled_command(0xA6);       // Normal display
    oled_command(0xA4);       // Display RAM

    oled_command(0x8D);       // Charge pump
    oled_command(0x14);

    oled_command(0xD9);       // Pre-charge
    oled_command(0xF1);

    oled_command(0xDB);       // VCOM
    oled_command(0x40);

    // Page addressing mode
    oled_command(0x20);
    oled_command(0x02);

    delay(100);

    oled_command(0xAF);       // Display ON
}


// ============================================================
// FRAMEBUFFER
// ============================================================

#define OLED_BUFFER_SIZE (OLED_WIDTH * OLED_HEIGHT / 8)

static uint8_t oled_buffer[OLED_BUFFER_SIZE];


// ============================================================
// SET PAGE + 4 COLUMN OFFSET
// ============================================================

static void oled_set_page(uint8_t page)
{
    oled_command(0xB0 | page);

    // --------------------------------------------------------
    // SSD1305 has 132 RAM columns.
    //
    // Visible 128-pixel display starts at RAM column 4.
    // --------------------------------------------------------

    uint8_t column = OLED_COL_OFFSET;

    // Lower nibble
    oled_command(0x00 | (column & 0x0F));

    // Upper nibble
    oled_command(0x10 | ((column >> 4) & 0x0F));
}


// ============================================================
// UPDATE DISPLAY
// ============================================================

void oled_update()
{
    uint8_t block[16];

    for (uint8_t page = 0; page < 4; page++)
    {
        oled_set_page(page);

        uint16_t base = page * OLED_WIDTH;

        for (uint8_t x = 0; x < OLED_WIDTH; x += 16)
        {
            for (uint8_t i = 0; i < 16; i++)
            {
                block[i] = oled_buffer[base + x + i];
            }

            oled_data(block, 16);
        }
    }
}


// ============================================================
// CLEAR
// ============================================================

void oled_clear()
{
    for (uint16_t i = 0; i < OLED_BUFFER_SIZE; i++)
        oled_buffer[i] = 0x00;

    oled_update();
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
        1U << (y % 8);

    if (on)
        oled_buffer[index] |= mask;
    else
        oled_buffer[index] &= (uint8_t)~mask;
}


// ============================================================
// FONT
// ============================================================

static const uint8_t font5x7[][5] =
{
    {0x00,0x00,0x00,0x00,0x00}, // SPACE
    {0x00,0x40,0x20,0x00,0x00}, // ,
    {0x08,0x08,0x08,0x08,0x08}, // -
    {0x00,0x60,0x60,0x00,0x00}, // .

    // 0-9
    {0x3E,0x51,0x49,0x45,0x3E},
    {0x00,0x42,0x7F,0x40,0x00},
    {0x42,0x61,0x51,0x49,0x46},
    {0x21,0x41,0x45,0x4B,0x31},
    {0x18,0x14,0x12,0x7F,0x10},
    {0x27,0x45,0x45,0x45,0x39},
    {0x3C,0x4A,0x49,0x49,0x30},
    {0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36},
    {0x06,0x49,0x49,0x29,0x1E},

    // A-Z
    {0x7E,0x11,0x11,0x11,0x7E},
    {0x7F,0x49,0x49,0x49,0x36},
    {0x3E,0x41,0x41,0x41,0x22},
    {0x7F,0x41,0x41,0x22,0x1C},
    {0x7F,0x49,0x49,0x49,0x41},
    {0x7F,0x09,0x09,0x09,0x01},
    {0x3E,0x41,0x49,0x49,0x7A},
    {0x7F,0x08,0x08,0x08,0x7F},
    {0x00,0x41,0x7F,0x41,0x00},
    {0x20,0x40,0x41,0x3F,0x01},
    {0x7F,0x08,0x14,0x22,0x41},
    {0x7F,0x40,0x40,0x40,0x40},
    {0x7F,0x02,0x0C,0x02,0x7F},
    {0x7F,0x04,0x08,0x10,0x7F},
    {0x3E,0x41,0x41,0x41,0x3E},
    {0x7F,0x09,0x09,0x09,0x06},
    {0x3E,0x41,0x51,0x21,0x5E},
    {0x7F,0x09,0x19,0x29,0x46},
    {0x46,0x49,0x49,0x49,0x31},
    {0x01,0x01,0x7F,0x01,0x01},
    {0x3F,0x40,0x40,0x40,0x3F},
    {0x1F,0x20,0x40,0x20,0x1F},
    {0x3F,0x40,0x38,0x40,0x3F},
    {0x63,0x14,0x08,0x14,0x63},
    {0x07,0x08,0x70,0x08,0x07},
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
                oled_pixel(
                    x + column,
                    y + row,
                    true
                );
            }
        }
    }
}


// ============================================================
// PRINT
// ============================================================

void oled_print(const char *text, uint8_t row)
{
    if (row >= 4)
        return;

    uint8_t x = 0;
    uint8_t y = row * 8;

    while (*text)
    {
        if (x + 5 >= OLED_WIDTH)
            break;

        oled_char(x, y, *text);

        x += 6;
        text++;
    }

    oled_update();
}


// ============================================================
// TEST
// ============================================================

void setup()
{
    pinMode(STATUS_LED, OUTPUT);

    // Active-low LED
    digitalWrite(STATUS_LED, LOW);

    // I2C
    OLED_WIRE.begin();

    // Keep the proven slow speed
    OLED_WIRE.setClock(50000);

    delay(200);

    // OLED
    oled_controller_init();

    // Clear framebuffer
    for (uint16_t i = 0; i < OLED_BUFFER_SIZE; i++)
        oled_buffer[i] = 0x00;

    // Test
    oled_print("HELLO", 0);
    oled_print("SSD1305", 1);
    oled_print("134.2 kHz", 2);
    oled_print("WORKING!", 3);

    while (1)
    {
        delay(1000);
    }
}


void loop()
{
}
