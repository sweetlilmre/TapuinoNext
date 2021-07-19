#pragma once

#include "InputHandler.h"
#include "LCDUtils.h"
#include "MenuTypes.h"
#include "OptionTypes.h"

namespace TapuinoNext
{
    class MenuHandler
    {
      public:
        MenuHandler(LCDUtils* utils, InputHandler* input);
        int Display(TheMenu* rootMenu);
        InputResponse DisplayValue(const char* header, ValueOption* valueOption);
        InputResponse DisplayToggle(const char* header, ToggleOption* toggleOption);
        InputResponse DisplayEnum(const char* header, EnumOption* enumOption);

        static void LinkSubMenu(TheMenu* theMenu, int entryIndex, TheMenu* subMenu)
        {
            theMenu->entries[entryIndex].data.nextMenu = subMenu;
            subMenu->parent = theMenu;
        }

        static void SetValueOtion(MenuEntry* menuEntry, int entryIndex, ValueOption* val)
        {
            menuEntry[entryIndex].data.valueOption = val;
        }
        static void SetToggleOption(MenuEntry* menuEntry, int entryIndex, ToggleOption* val)
        {
            menuEntry[entryIndex].data.toggleOption = val;
        }
        static void SetEnumOption(MenuEntry* menuEntry, int entryIndex, EnumOption* val)
        {
            menuEntry[entryIndex].data.enumOption = val;
        }
        static void SetAction(MenuEntry* menuEntry, int entryIndex, ActionCallback* val)
        {
            menuEntry[entryIndex].data.actionCB = val;
        }

      private:
        LCDUtils* utils;
        InputHandler* input;
    };
} // namespace TapuinoNext
