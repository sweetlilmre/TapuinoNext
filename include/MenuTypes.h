#pragma once
#include "OptionTypes.h"

namespace TapuinoNext
{
    class ActionCallback
    {
      public:
        virtual void OnAction() = 0;
    };

    struct TheMenu;

    enum class MenuEntryType
    {
        SubMenuEntry,
        ActionEntry,
        IndexEntry,
        ToggleEntry,
        ValueEntry,
        EnumEntry
    };

    union MenuEntryData
    {
        ValueOption* valueOption;
        ActionCallback* actionCB;
        ToggleOption* toggleOption;
        EnumOption* enumOption;
        TheMenu* nextMenu;
    };

    struct MenuEntry
    {
        MenuEntryType type;
        const char* text;
        MenuEntryData data;
    };

    struct TheMenu
    {
        const char* header; // Menu Header text
        MenuEntry* entries;
        uint8_t count;
        uint8_t curIndex;
        TheMenu* parent;
    };
} // namespace TapuinoNext
