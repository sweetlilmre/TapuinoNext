//#ifdef LCD_USE_SSD1306_OLED_MODULE

#include "OLED_1306.h"
#include "OLEDFont8x8.h"
#include <Arduino.h>
#include <Wire.h>

using namespace TapuinoNext;

#define SSD1306_DISPLAY_OFF 0xAE
#define SSD1306_SET_DISPLAY_CLOCK_DIV 0xD5
#define SSD1306_SET_MULTIPLEX 0xA8
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_SET_MEMORYMODE 0x20
#define SSD1306_MEMORYMODE_HORIZONTAL 0x00
#define SSD1306_MEMORYMODE_VERTICAL 0x01
#define SSD1306_MEMORYMODE_PAGE 0x02

// column or y mapping
#define SSD1306_SEGREMAPNORMAL 0xA0
#define SSD1306_SEGREMAPINV 0xA1

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAY_NORMAL 0xA6
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETSTARTPAGE 0xB0
#define SSD1306_LOWCOLUMNADDR 0x00
#define SSD1306_HIGHCOLUMNADDR 0x10

#define SSD1306_DEACTIVATE_SCROLL 0x2E

#define SSD1306_SETCOLUMNADDR 0x21
#define SSD1306_SETPAGEADDR 0x22

// clang-format off
// Init Sequence
const uint8_t ssd1306_init_sequence[] = {
    SSD1306_DISPLAY_OFF,                    // 0xAE Display OFF (sleep mode)
    SSD1306_SET_DISPLAY_CLOCK_DIV, 0x80,    // 0xD5 Set display clock divide ratio/oscillator frequency
#if defined LCD_SSD1306_128x64
    SSD1306_SET_MULTIPLEX, 0x3F,            // 0xA8 Set multiplex ratio (1 to 64)
#elif defined LCD_SSD1306_128x32
    SSD1306_SET_MULTIPLEX, 0x1F,            // 0xA8 Set multiplex ratio (1 to 64)
#endif
    SSD1306_SETDISPLAYOFFSET, 0x00,         // 0xD3 Set display offset. 00 = no offset
    SSD1306_SETSTARTLINE | 0x00,            // 0x40 Set start line address
    SSD1306_CHARGEPUMP, 0x14,               // 0x8D Set DC-DC enable: internal VCC
    SSD1306_SET_MEMORYMODE,                 // 0x20 Set Memory Addressing Mode
    SSD1306_MEMORYMODE_PAGE,                //      00b = Horizontal Addressing Mode;   01b = Vertical Addressing Mode;
                                            //      10b = Page Addressing Mode (RESET); 11b = Invalid
    SSD1306_SEGREMAPINV,                    // 0xA0 Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
    SSD1306_COMSCANDEC,                     // 0xC8 Set COM Output Scan Direction - descending
#if defined LCD_SSD1306_128x64
    SSD1306_SETCOMPINS, 0x12,               // 0xDA Set com pins hardware configuration
#elif defined LCD_SSD1306_128x32
    SSD1306_SETCOMPINS, 0x02,               // 0xDA Set com pins hardware configuration
#endif

    SSD1306_SETCONTRAST, 0x3F,              // 0x81 Set contrast control register
    SSD1306_SETPRECHARGE, 0x22,             // 0xD9 Set pre-charge period
    SSD1306_SETVCOMDETECT, 0x20,            // 0xDB Set vcomh 0x20,0.77xVcc
    SSD1306_DISPLAYALLON_RESUME,            // 0xA4 Output RAM to Display 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
    SSD1306_DISPLAY_NORMAL,                 // 0xA6 Set display mode. A6=Normal; A7=Inverse
    SSD1306_DISPLAYON,                      // 0xAF Display ON in normal mode
    SSD1306_DEACTIVATE_SCROLL,               // 0x2E
    SSD1306_SETPAGEADDR,
    0,
    0x3F,
    SSD1306_SETCOLUMNADDR,
    0,
    0x7F

};
// clang-format on

OLED_1306::OLED_1306(OptionEventHandler* optionEventHandler) : LCD(optionEventHandler)
{
}

OLED_1306::~OLED_1306()
{
}

void OLED_1306::OnChange(OptionTagId tag, IOptionType* option)
{
}

void OLED_1306::Init(uint8_t I2CAddr)
{
    addr = I2CAddr;
    curRow = 0;
    curCol = 0;
    displayCursor = 0;

    Wire.begin();
    delayMicroseconds(100);

    for (uint8_t i = 0; i < sizeof(ssd1306_init_sequence); i++)
    {
        SendCommand(ssd1306_init_sequence[i]);
    }
    FillScreen(0);
    memset(buffer, 32, I2C_DISP_ROWS * I2C_DISP_COLS);
}

void OLED_1306::Cursor()
{
    uint8_t curValue = buffer[curRow * I2C_DISP_COLS + curCol];
    displayCursor = 1;
    WriteRaw(curValue, 1);
}

void OLED_1306::NoCursor()
{
    uint8_t curValue = buffer[curRow * I2C_DISP_COLS + curCol];
    displayCursor = 0;
    WriteRaw(curValue, 0);
}

void OLED_1306::Backlight()
{
}

void OLED_1306::NoBacklight()
{
}

void OLED_1306::WriteRaw(uint8_t value, bool drawCursor)
{
    if (value < 32)
    {
        return;
    }

    uint16_t fontIndex = (value - 32) << 3; // index into font data, each char is 8 bytes, characters start at ASCII space (32)
    uint8_t pixelCol = curCol << 3;         // convert to pixel position: character column * 8

    SendCommandStart();
    Wire.write(SSD1306_SETSTARTPAGE + curRow);
    Wire.write((pixelCol & 0x0f) | SSD1306_LOWCOLUMNADDR);
    Wire.write(((pixelCol & 0xf0) >> 4) | SSD1306_HIGHCOLUMNADDR);
    Wire.endTransmission();

    SendDataStart();
    // write 1 column of the character per iteration
    for (uint8_t i = 0; i < 8; i++)
    {
        uint8_t v = font8x8[fontIndex + i];
        // add underline for cursor: last pixel in the column
        if (drawCursor)
            v |= 0x80;
        Wire.write(v);
    }
    Wire.endTransmission();
}

void OLED_1306::SetCursor(uint8_t col, uint8_t row)
{
    if ((row < I2C_DISP_ROWS) && (col < I2C_DISP_COLS))
    {
        if (displayCursor)
        {
            uint8_t curValue = buffer[curRow * I2C_DISP_COLS + curCol];
            WriteRaw(curValue, false);
            curValue = buffer[row * I2C_DISP_COLS + col];
            WriteRaw(curValue, true);
        }

        curCol = col;
        curRow = row;
    }
}

void OLED_1306::Write(uint8_t value)
{
    WriteRaw(value, 0);

    buffer[curRow * I2C_DISP_COLS + curCol] = value;
    curCol++;
    if (curCol >= I2C_DISP_COLS)
    {
        curCol = 0;
        curRow = (curRow + 1) % I2C_DISP_ROWS;
    }
    if (displayCursor)
    {
        uint8_t curValue = buffer[curRow * I2C_DISP_COLS + curCol];
        WriteRaw(curValue, 1);
    }
}

void OLED_1306::Print(const char* s)
{
    while (*s)
    {
        Write(*s++);
    }
}

// ----------------------------------------------------------------------------
void OLED_1306::FillScreen(uint8_t fill)
{
    for (uint8_t m = 0; m < 8; m++)
    {
        SendCommandStart();
        Wire.write(SSD1306_SETSTARTPAGE + m); // page0 - page7
        Wire.write(SSD1306_LOWCOLUMNADDR);    // low column start address
        Wire.write(SSD1306_HIGHCOLUMNADDR);   // high column start address
        Wire.endTransmission();

        SendDataStart();
        for (uint8_t n = 0; n < 128; n++)
        {
            Wire.write(fill);
            if (n % 16 == 15)
            {
                Wire.endTransmission();
                SendDataStart();
            }
        }
        Wire.endTransmission();
    }
}

void OLED_1306::SendCommandStart()
{
    Wire.beginTransmission(addr);
    Wire.write(0x00); // command
}

void OLED_1306::SendCommand(uint8_t command)
{
    SendCommandStart();
    Wire.write(command);
    Wire.endTransmission();
}

void OLED_1306::SendDataStart()
{
    Wire.beginTransmission(addr);
    Wire.write(0x40); // data
}

//#endif
