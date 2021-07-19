#pragma once
#include "LCD.h"

namespace TapuinoNext
{
    class LCD_HD44780 : public LCD
    {
      public:
        LCD_HD44780(OptionEventHandler* optionEventHandler);
        ~LCD_HD44780();
        void OnChange(OptionTagId tag, IOptionType* option);
        void Init(uint8_t I2CAddr);
        void Cursor();
        void NoCursor();
        void Backlight();
        void NoBacklight();
        void SetCursor(uint8_t col, uint8_t row);
        void Print(const char* msg);
        void Write(uint8_t value);

      private:
        void Send(uint8_t value, uint8_t mode);
        void Write4bits(uint8_t value);
        void ExpanderWrite(uint8_t _data);
        void PulseEnable(uint8_t _data);
        void Display();
        void NoDisplay();
        void Clear();
        void Home();
        void CreateChar(uint8_t location, uint8_t charmap[]);
        void Command(uint8_t value);

        uint8_t addr;
        uint8_t displayFunction;
        uint8_t displayControl;
        uint8_t displayMode;
        uint8_t numLines;
        uint8_t cols;
        uint8_t rows;
        uint8_t backlightVal;
    };
} // namespace TapuinoNext
