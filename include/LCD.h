#pragma once

#include <inttypes.h>
#include "OptionEventHandler.h"

#define DIRECTORY_INDICATOR 0x7E
#define ENTER_INDICATOR 0x81

namespace TapuinoNext
{
    class LCD : public IChangeHandler
    {
      public:
        LCD(OptionEventHandler* optionEventHandler) : optionEventHandler(optionEventHandler){};
        virtual void Init(uint8_t I2CAddr) = 0;
        virtual void Cursor() = 0;
        virtual void NoCursor() = 0;
        virtual void Backlight() = 0;
        virtual void NoBacklight() = 0;
        virtual void SetCursor(uint8_t col, uint8_t row) = 0;
        virtual void Print(const char* msg) = 0;
        virtual void Write(uint8_t value) = 0;

      protected:
        OptionEventHandler* optionEventHandler;
    };
} // namespace TapuinoNext
