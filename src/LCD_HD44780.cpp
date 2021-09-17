#include <Arduino.h>
#include <Wire.h>

#include "LCD_HD44780.h"
#include "config.h"

using namespace TapuinoNext;

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

#define REGISTER_ENABLE B00000100    // Enable bit
#define REGISTER_READWRITE B00000010 // Read/Write bit
#define REGISTER_SELECT B00000001    // Register select bit

LCD_HD44780::LCD_HD44780(OptionEventHandler* optionEventHandler) : LCD(optionEventHandler)
{
    optionEventHandler->Subscribe(OptionTagId::Backlight, this);
}

LCD_HD44780::~LCD_HD44780()
{
}

void LCD_HD44780::OnChange(OptionTagId tag, IOptionType* option)
{
    Serial.printf("LCD_HD44780::OnChange called\n");
    switch (tag)
    {
        case OptionTagId::Backlight:
        {
            if (option->GetType() != ConfigOptionType::ConfigToggle)
            {
                Serial.println("ERROR! Incorrect option type passed to LCD_HD44780::OnChange!");
                return;
            }
            ToggleOption* tog = (ToggleOption*) option;
            tog->GetValue() ? Backlight() : NoBacklight();
            break;
        }
        default:
            break;
    }
}

void LCD_HD44780::Init(uint8_t I2CAddr)
{
    addr = I2CAddr;
    cols = I2C_DISP_COLS;
    rows = I2C_DISP_ROWS;
    backlightVal = LCD_NOBACKLIGHT;

    Wire.begin();
    displayFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

    if (rows > 1)
    {
        displayFunction |= LCD_2LINE;
    }
    numLines = rows;

    // HD44780U.PDF, pg 46:
    // (1) Wait for more than 15 ms after VCC rises to 4.5 V
    // (2) Wait for more than 40 ms after VCC rises to 2.7 V
    delay(50);

    // Reset expander, backlight off
    ExpanderWrite(0);
    delay(1000);

    // HD44780U.PDF, pg 46:
    // Function set: 8 bit mode
    // RS R/W DB7 DB6 DB5 DB4
    //  0   0   0   0   1   1
    // Wait for more than 4.1 ms
    Write4bits(0x03 << 4);
    delayMicroseconds(4500);

    // HD44780U.PDF, pg 46:
    // Function set: 8 bit mode
    // RS R/W DB7 DB6 DB5 DB4
    //  0   0   0   0   1   1
    // Wait for more than 100 us
    Write4bits(0x03 << 4);
    delayMicroseconds(150);

    // HD44780U.PDF, pg 46:
    // Function set: 8 bit mode
    // RS R/W DB7 DB6 DB5 DB4
    //  0   0   0   0   1   1
    Write4bits(0x03 << 4);

    // HD44780U.PDF, pg 46:
    // Function set: 4 bit mode
    // RS R/W DB7 DB6 DB5 DB4
    //  0   0   0   0   1   0
    Write4bits(0x02 << 4);

    Command(LCD_FUNCTIONSET | displayFunction);

    displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    Display();
    Clear();

    displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    Command(LCD_ENTRYMODESET | displayMode);

    Home();

    // clang-format off

    // create backslash char for the spinner UI
    uint8_t backslashChar[8] = 
    {
        0b00000, 
        0b10000, 
        0b01000, 
        0b00100, 
        0b00010, 
        0b00001, 
        0b00000, 
        0b00000
    };

    // create enter char for the name entry UI
    uint8_t enterChar[8] = 
    {
        0b00000,
        0b00001,
        0b00001,
        0b00101,
        0b01001,
        0b11111,
        0b01000,
        0b00100
    };

    // clang-format on

    CreateChar(0, backslashChar);
    CreateChar(1, enterChar);
}

void LCD_HD44780::Backlight()
{
    backlightVal = LCD_BACKLIGHT;
    ExpanderWrite(backlightVal);
}

void LCD_HD44780::NoBacklight()
{
    backlightVal = LCD_NOBACKLIGHT;
    ExpanderWrite(backlightVal);
}

void LCD_HD44780::Print(const char* msg)
{
    uint8_t v;
    while ((v = *msg++))
    {
        Write(v);
    }
}

void LCD_HD44780::Clear()
{
    Command(LCD_CLEARDISPLAY);
    delayMicroseconds(2000);
}

void LCD_HD44780::Home()
{
    Command(LCD_RETURNHOME);
    // HD44780U.PDF, pg 24:
    // command execution time 37us
    // Return home 1.52ms, already waited 50 in PulseEnable()
    delayMicroseconds(1500);
}

void LCD_HD44780::SetCursor(uint8_t col, uint8_t row)
{
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > numLines)
    {
        row = numLines - 1;
    }
    Command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCD_HD44780::NoDisplay()
{
    displayControl &= ~LCD_DISPLAYON;
    Command(LCD_DISPLAYCONTROL | displayControl);
}

void LCD_HD44780::Display()
{
    displayControl |= LCD_DISPLAYON;
    Command(LCD_DISPLAYCONTROL | displayControl);
}

void LCD_HD44780::NoCursor()
{
    displayControl &= ~LCD_CURSORON;
    Command(LCD_DISPLAYCONTROL | displayControl);
}

void LCD_HD44780::Cursor()
{
    displayControl |= LCD_CURSORON;
    Command(LCD_DISPLAYCONTROL | displayControl);
}

void LCD_HD44780::CreateChar(uint8_t location, uint8_t charmap[])
{
    int i;
    location &= 0x7;
    Command(LCD_SETCGRAMADDR | (location << 3));
    for (i = 0; i < 8; i++)
    {
        Write(charmap[i]);
    }
}

void LCD_HD44780::Write(uint8_t value)
{
    // special handling for user defined chars
    if (value == '\\')
        value = 0;
    if (value == ENTER_INDICATOR)
        value = 1;

    Send(value, REGISTER_SELECT);
}

void LCD_HD44780::Command(uint8_t value)
{
    Send(value, 0);
}

void LCD_HD44780::Send(uint8_t value, uint8_t mode)
{
    uint8_t highnib = value & 0xF0;
    uint8_t lownib = (value << 4) & 0xF0;
    Write4bits(highnib | mode);
    Write4bits(lownib | mode);
}

void LCD_HD44780::Write4bits(uint8_t value)
{
    ExpanderWrite(value);
    PulseEnable(value);
}

void LCD_HD44780::ExpanderWrite(uint8_t data)
{
    Wire.beginTransmission(addr);
    Wire.write((int) (data) | backlightVal);
    Wire.endTransmission();
}

void LCD_HD44780::PulseEnable(uint8_t data)
{
    ExpanderWrite(data | REGISTER_ENABLE);
    // HD44780U.PDF, pg 49:
    // Item                            Symbol   Min
    // Enable pulse width (high level) PWEH     450
    // 1000 should do it
    delayMicroseconds(1);

    ExpanderWrite(data & ~REGISTER_ENABLE);
    // HD44780U.PDF, pg 24:
    // command execution time 37us
    delayMicroseconds(50);
}
