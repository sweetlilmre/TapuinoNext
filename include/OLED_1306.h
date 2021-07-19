#pragma once
#include "LCD.h"
#include "config.h"

namespace TapuinoNext
{
    class OLED_1306 : public LCD
    {
      public:
        OLED_1306(OptionEventHandler* optionEventHandler);
        ~OLED_1306();
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
        void WriteRaw(uint8_t value, bool drawCursor);
        void FillScreen(uint8_t fill);
        void SendCommandStart();
        void SendCommand(uint8_t command);
        void SendDataStart();

        uint8_t addr;
        uint8_t curRow, curCol;
        uint8_t buffer[I2C_DISP_ROWS * I2C_DISP_COLS];
        uint8_t displayCursor;
    };
} // namespace TapuinoNext