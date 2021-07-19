#include <Arduino.h>
#include "OptionEventHandler.h"

using namespace TapuinoNext;
using namespace std;

OptionEventHandler::OptionEventHandler()
{
}

OptionEventHandler::~OptionEventHandler()
{
}

void OptionEventHandler::Notify(OptionTagId tag, IOptionType* optionType)
{
    Serial.printf("OptionEventHandler::Notify called: %d\n", (uint8_t) tag);

    for (int i = 0; i < handlers[(uint8_t) tag].size(); i++)
    {
        handlers[(uint8_t) tag][i]->OnChange(tag, optionType);
    }
}

void OptionEventHandler::Subscribe(OptionTagId tag, IChangeHandler* handler)
{
    handlers[(uint8_t) tag].push_back(handler);
}
