#include <stdlib.h>
#ifdef ESP32
#include "stdlib_noniso.h"
#endif

#include "Menu.h"
#include <Arduino.h>

using namespace TapuinoNext;

MenuHandler::MenuHandler(LCDUtils* utils, InputHandler* input)
{
    this->utils = utils;
    this->input = input;
}

int MenuHandler::Display(TheMenu* rootMenu)
{
    MenuEntry* curEntry;

    if (rootMenu == NULL)
    {
        Serial.println("NULL rootMenu in MenuHandler::Display()!");
        return (-1);
    }

    utils->Title(rootMenu->header);
    curEntry = &rootMenu->entries[rootMenu->curIndex];

    while (true)
    {
        utils->Status(curEntry->text);
        InputResponse resp;
        while ((resp = input->GetInput()) == InputResponse::None)
            ;
        switch (resp)
        {
            case InputResponse::Next:
            {
                rootMenu->curIndex++;
                if (rootMenu->curIndex == rootMenu->count)
                    rootMenu->curIndex = 0;
                curEntry = &rootMenu->entries[rootMenu->curIndex];
                utils->Status(curEntry->text);
            }
            break;

            case InputResponse::Prev:
            {
                if (rootMenu->curIndex == 0)
                    rootMenu->curIndex = rootMenu->count;
                rootMenu->curIndex--;
                curEntry = &rootMenu->entries[rootMenu->curIndex];
                utils->Status(curEntry->text);
            }
            break;

            case InputResponse::Select:
            {
                switch (curEntry->type)
                {
                    case MenuEntryType::SubMenuEntry:
                    {
                        rootMenu = curEntry->data.nextMenu;
                        utils->Title(rootMenu->header);
                        curEntry = &rootMenu->entries[rootMenu->curIndex];
                        break;
                    }

                    case MenuEntryType::ActionEntry:
                    {
                        if (curEntry->data.actionCB != NULL)
                        {
                            curEntry->data.actionCB->OnAction();
                            utils->Title(rootMenu->header);
                        }
                        break;
                    }

                    case MenuEntryType::IndexEntry:
                    {
                        return (rootMenu->curIndex);
                    }

                    case MenuEntryType::ValueEntry:
                    {
                        ValueOption* valOpt = curEntry->data.valueOption;
                        if (valOpt != NULL)
                        {
                            if (InputResponse::Select == DisplayValue(curEntry->text, valOpt))
                            {
                                valOpt->Commit();
                            }
                            utils->Title(rootMenu->header);
                        }
                        break;
                    }

                    case MenuEntryType::ToggleEntry:
                    {
                        ToggleOption* togOpt = curEntry->data.toggleOption;
                        if (togOpt != NULL)
                        {
                            if (InputResponse::Select == DisplayToggle(curEntry->text, togOpt))
                            {
                                togOpt->Commit();
                            }
                            utils->Title(rootMenu->header);
                        }
                        break;
                    }

                    case MenuEntryType::EnumEntry:
                    {
                        EnumOption* enumOpt = curEntry->data.enumOption;

                        if (enumOpt != NULL)
                        {
                            if (InputResponse::Select == DisplayEnum(curEntry->text, enumOpt))
                            {
                                enumOpt->Commit();
                            }
                            utils->Title(rootMenu->header);
                        }
                        break;
                    }
                }
            }
            break;

            case InputResponse::Abort:
                if (rootMenu->parent != NULL)
                {
                    rootMenu = rootMenu->parent;
                    utils->Title(rootMenu->header);
                    curEntry = &rootMenu->entries[rootMenu->curIndex];
                }
                else
                {
                    // abort value for code using the MenuEntryType::IndexEntry
                    return (-1);
                }
                break;
            default:
                break;
        }
    }
}

InputResponse MenuHandler::DisplayValue(const char* header, ValueOption* valueOption)
{
    utils->Title(header);
    char valueBuffer[10];

    while (true)
    {
        utils->Status(ltoa(valueOption->GetValue(), valueBuffer, 10));
        InputResponse resp;
        while ((resp = input->GetInput()) == InputResponse::None)
        {
        }

        switch (resp)
        {
            case InputResponse::Next:
                valueOption->IncValue();
                break;

            case InputResponse::Prev:
                valueOption->DecValue();
                break;

            case InputResponse::Select:
                return (resp);

            case InputResponse::Abort:
                valueOption->Revert();
                return (resp);

            default:
                break;
        }
    }
}

InputResponse MenuHandler::DisplayToggle(const char* header, ToggleOption* toggleOption)
{
    utils->Title(header);

    while (true)
    {
        utils->Status(toggleOption->GetToggleText());
        InputResponse resp;
        while ((resp = input->GetInput()) == InputResponse::None)
            ;
        switch (resp)
        {
            case InputResponse::Next:
                toggleOption->Toggle();
                break;

            case InputResponse::Prev:
                toggleOption->Toggle();
                break;

            case InputResponse::Select:
                return (resp);

            case InputResponse::Abort:
                toggleOption->Revert();
                return (resp);
            default:
                break;
        }
    }
}

InputResponse MenuHandler::DisplayEnum(const char* header, EnumOption* enumOption)
{
    utils->Title(header);

    while (true)
    {
        utils->Status(enumOption->GetEnumText());
        InputResponse resp;
        while ((resp = input->GetInput()) == InputResponse::None)
            ;
        switch (resp)
        {
            case InputResponse::Next:
                enumOption->Next();
                break;

            case InputResponse::Prev:
                enumOption->Prev();
                break;

            case InputResponse::Select:
                return (resp);

            case InputResponse::Abort:
                enumOption->Revert();
                return (resp);
            default:
                break;
        }
    }
}