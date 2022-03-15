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
    {MenuEntryType::ActionEntry, S_UPDATE, NULL},
};

TheMenu optionsMenu = {S_OPTIONS, (MenuEntry*) optionsMenuEntries, 3, 0, NULL};

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

Options::Options(FileLoader* fileLoader, IChangeNotify* notify, MenuHandler* menuHandler, ActionCallback* updateCallback)
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
    allOptions.push_back(&btnHoldTime);
    allOptions.push_back(&btnClickTime);
    allOptions.push_back(&tickerTime);
    allOptions.push_back(&tickerHoldTime);
    allOptions.push_back(&ntscPAL);
    allOptions.push_back(&autoPlay);
    allOptions.push_back(&backlight);
    allOptions.push_back(&machineType);

    MenuHandler::LinkSubMenu(&optionsMenu, 0, &optionsInputMenu);
    MenuHandler::LinkSubMenu(&optionsMenu, 1, &optionsMachineMenu);
    MenuHandler::SetAction(optionsMenuEntries, 2, updateCallback);

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
        while (configFile.available())
        {
            memset(buf, 0, 256);
            configFile.readBytesUntil('\n', buf, 255);
            Serial.println(buf);
            char* tagChars = buf;
            char* valChars = strrchr(buf, ':');
            if (!valChars)
            {
                continue;
            }

            // terminate
            *valChars++ = 0;
            // and trim
            while (*valChars != '\0' && (*valChars == ' ' || *valChars == '\t'))
                valChars++;

            Serial.println(tagChars);
            Serial.println(valChars);

            for (int i = 0; i < allOptions.size(); i++)
            {
                IOptionType* opt = allOptions[i];
                const char* szTag = TagIdToString(opt->GetTag());
                if (0 == strcmp(tagChars, szTag))
                {
                    Serial.printf("Found tag (%s): ", szTag);
                    switch (opt->GetType())
                    {
                        case ConfigOptionType::ConfigValue:
                        {
                            Serial.print("ValueOption ");
                            ValueOption* optVal = (ValueOption*) opt;
                            optVal->SetValue((uint32_t) atoi(valChars));
                            optVal->Commit();
                        }
                        break;
                        case ConfigOptionType::ConfigToggle:
                        {
                            Serial.print("ToggleOption ");
                            ToggleOption* optTog = (ToggleOption*) opt;
                            optTog->SetValue(valChars[0] == 't');
                            optTog->Commit();
                        }
                        break;
                        case ConfigOptionType::ConfigEnum:
                        {
                            Serial.print("EnumOption ");
                            EnumOption* optEnum = (EnumOption*) opt;
                            optEnum->ParseValue(valChars);
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
        char buf[256];
        for (int i = 0; i < allOptions.size(); i++)
        {
            memset(buf, 0, 256);
            IOptionType* opt = allOptions[i];
            const char* tagString = TagIdToString(opt->GetTag());
            switch (opt->GetType())
            {
                case ConfigOptionType::ConfigValue:
                {
                    ValueOption* optVal = (ValueOption*) opt;
                    snprintf(buf, 255, "%s: %u\n", tagString, optVal->GetValue());
                }
                break;
                case ConfigOptionType::ConfigToggle:
                {
                    ToggleOption* optTog = (ToggleOption*) opt;
                    snprintf(buf, 255, "%s: %s\n", tagString, optTog->GetValue() ? "true" : "false");
                }
                break;
                case ConfigOptionType::ConfigEnum:
                {
                    EnumOption* optEnum = (EnumOption*) opt;
                    snprintf(buf, 255, "%s: %s\n", tagString, optEnum->GetEnumText());
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
