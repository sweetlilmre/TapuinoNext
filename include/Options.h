#pragma once
#include <inttypes.h>
#include "FileLoader.h"
#include "MenuTypes.h"
#include "OptionTypes.h"
#include "InputHandler.h"
#include "Menu.h"

namespace TapuinoNext
{
    class Options
    {
      public:
        Options(FileLoader* fileLoader, IChangeNotify* notify, MenuHandler* menuHandler);
        void LoadOptions();
        void SaveOptions();
        void OnAction();

        ValueOption btnClickTime;
        ValueOption btnHoldTime;
        ValueOption tickerTime;
        ValueOption tickerHoldTime;
        ToggleOption ntscPAL;
        ToggleOption autoPlay;
        ToggleOption backlight;
        EnumOption machineType;

      protected:
        const char* TagIdToString(OptionTagId id);
        FileLoader* fileLoader;
        MenuHandler* menuHandler;
        IOptionType* allOptions[(uint8_t) OptionTagId::LAST];
    };
} // namespace TapuinoNext
