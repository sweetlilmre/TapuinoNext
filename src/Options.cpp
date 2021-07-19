#include "Options.h"
#include "FileLoader.h"
#include "Lang.h"
#include "Menu.h"
#include "SharedTypes.h"
#include <stddef.h>

using namespace TapuinoNext;

MenuEntry optionsMenuEntries[] = {
    {MenuEntryType::SubMenuEntry, S_INPUT_AND_UI, NULL},
    {MenuEntryType::SubMenuEntry, S_MACHINE, NULL},
};

TheMenu optionsMenu = {S_OPTIONS, (MenuEntry*) optionsMenuEntries, 2, 0, NULL};

MenuEntry optionsInputMenuEntries[] = {
    {MenuEntryType::ValueEntry, S_BTN_CLICK_TIME, NULL}, {MenuEntryType::ValueEntry, S_BTN_HOLD_TIME, NULL}, {MenuEntryType::ValueEntry, S_TICKER_TIME, NULL},
    {MenuEntryType::ToggleEntry, S_AUTOPLAY, NULL},      {MenuEntryType::ToggleEntry, S_BACKLIGHT, NULL},
};

MenuEntry optionsMachineMenuEntries[] = {
    {MenuEntryType::ToggleEntry, S_VIDEO_MODE, NULL},
    {MenuEntryType::EnumEntry, S_MACHINE_TYPE, NULL},
};

TheMenu optionsInputMenu = {S_INPUT_AND_UI, (MenuEntry*) optionsInputMenuEntries, 5, 0, &optionsMenu};
TheMenu optionsMachineMenu = {S_MACHINE, (MenuEntry*) optionsMachineMenuEntries, 2, 0, &optionsMenu};

Options::Options(FileLoader* fileLoader, IChangeNotify* notify, MenuHandler* menuHandler)
    : btnClickTime(OptionTagId::ButtonClickTime, notify, 100, 100, 500, 10),
      btnHoldTime(OptionTagId::ButtonHoldTime, notify, 300, 500, 2000, 50),
      tickerTime(OptionTagId::TickerTime, notify, 500, 500, 2000, 50),
      tickerHoldTime(OptionTagId::TickerHoldTime, notify, 1000, 1000, 5000, 500),
      ntscPAL(OptionTagId::IsNTSC, notify, false, "PAL", "NTSC"),
      autoPlay(OptionTagId::AutoPlay, notify, true, S_FALSE, S_TRUE),
      backlight(OptionTagId::Backlight, notify, false, S_OFF, S_ON),
      machineType(OptionTagId::Machine, notify, machineTypeNames, 3, 0),
      fileLoader(fileLoader),
      menuHandler(menuHandler)
{
    int index = 0;
    allOptions[index++] = &btnHoldTime;
    allOptions[index++] = &btnClickTime;
    allOptions[index++] = &tickerTime;
    allOptions[index++] = &tickerHoldTime;
    allOptions[index++] = &ntscPAL;
    allOptions[index++] = &autoPlay;
    allOptions[index++] = &backlight;
    allOptions[index++] = &machineType;

    MenuHandler::LinkSubMenu(&optionsMenu, 0, &optionsInputMenu);
    MenuHandler::LinkSubMenu(&optionsMenu, 1, &optionsMachineMenu);

    MenuHandler::SetValueOtion(optionsInputMenuEntries, 0, &btnClickTime);
    MenuHandler::SetValueOtion(optionsInputMenuEntries, 1, &btnHoldTime);
    MenuHandler::SetValueOtion(optionsInputMenuEntries, 2, &tickerTime);
    MenuHandler::SetToggleOption(optionsInputMenuEntries, 3, &autoPlay);
    MenuHandler::SetToggleOption(optionsInputMenuEntries, 4, &backlight);

    MenuHandler::SetToggleOption(optionsMachineMenuEntries, 0, &ntscPAL);
    MenuHandler::SetEnumOption(optionsMachineMenuEntries, 1, &machineType);
}

const char* Options::TagIdToString(OptionTagId id)
{
    switch (id)
    {
        case OptionTagId::ButtonClickTime:
            return "ButtonClickTime";
            break;
        case OptionTagId::ButtonHoldTime:
            return "ButtonHoldTime";
            break;
        case OptionTagId::TickerTime:
            return "TickerTime";
            break;
        case OptionTagId::TickerHoldTime:
            return "TickerHoldTime";
            break;
        case OptionTagId::IsNTSC:
            return "IsNTSC";
            break;
        case OptionTagId::AutoPlay:
            return "AutoPlay";
            break;
        case OptionTagId::Backlight:
            return "Backlight";
            break;
        case OptionTagId::Machine:
            return "Machine";
            break;
        case OptionTagId::LAST:
        default:
            return "";
            break;
    }
}

void Options::OnAction()
{
    menuHandler->Display(&optionsMenu);
    SaveOptions();
}

void Options::LoadOptions()
{
    File configFile;
    char buf[256];

    if (ErrorCodes::OK == fileLoader->OpenFile("/TapuinoNext.cfg", configFile))
    {
        int numOptions = sizeof(allOptions) / sizeof(IOptionType*);
        while (configFile.available())
        {
            memset(buf, 0, 256);
            configFile.readBytesUntil('\n', buf, 256);
            Serial.println(buf);
            char* tagChars = buf;
            char* valChars = strrchr(buf, ':');
            if (!valChars)
            {
                continue;
            }
            *valChars++ = 0;
            String tagStr = tagChars;
            tagStr.trim();
            String valStr = valChars;
            valStr.trim();
            Serial.println(tagStr.c_str());
            Serial.println(valStr.c_str());

            for (int i = 0; i < numOptions; i++)
            {
                IOptionType* opt = allOptions[i];
                if (0 == strcmp(tagStr.c_str(), TagIdToString(opt->GetTag())))
                {
                    Serial.println("found tag");
                    switch (opt->GetType())
                    {
                        case ConfigOptionType::ConfigValue:
                        {
                            ValueOption* optVal = (ValueOption*) opt;
                            optVal->SetValue((uint32_t) atoi(valStr.c_str()));
                            optVal->Commit();
                        }
                        break;
                        case ConfigOptionType::ConfigToggle:
                        {
                            ToggleOption* optTog = (ToggleOption*) opt;
                            optTog->SetValue(valStr.charAt(0) == 't');
                            optTog->Commit();
                        }
                        break;
                        case ConfigOptionType::ConfigEnum:
                        {
                            EnumOption* optEnum = (EnumOption*) opt;
                            optEnum->ParseValue(valStr.c_str());
                            optEnum->Commit();
                            break;
                        }
                    }
                }
            }
        }
        configFile.close();
    }
}

void Options::SaveOptions()
{
    File configFile;
    Serial.println("saving options");
    if (ErrorCodes::OK == fileLoader->CreateFile("/TapuinoNext.cfg", configFile))
    {
        Serial.println("file opened");
        int numOptions = sizeof(allOptions) / sizeof(IOptionType*);
        char buf[256];
        for (int i = 0; i < numOptions; i++)
        {
            IOptionType* opt = allOptions[i];
            const char* tagString = TagIdToString(opt->GetTag());
            switch (opt->GetType())
            {
                case ConfigOptionType::ConfigValue:
                {
                    ValueOption* optVal = (ValueOption*) opt;
                    snprintf(buf, 256, "%s: %u\n", tagString, optVal->GetValue());
                }
                break;
                case ConfigOptionType::ConfigToggle:
                {
                    ToggleOption* optTog = (ToggleOption*) opt;
                    snprintf(buf, 256, "%s: %s\n", tagString, optTog->GetValue() ? "true" : "false");
                }
                break;
                case ConfigOptionType::ConfigEnum:
                {
                    EnumOption* optEnum = (EnumOption*) opt;
                    snprintf(buf, 256, "%s: %s\n", tagString, optEnum->GetEnumText());
                    break;
                }
            }

            int len = strlen(buf);
            Serial.printf("buf len: %d: ", len);
            Serial.println(buf);
            configFile.write((const uint8_t*) buf, len);
        }
        configFile.close();
    }
}
