#pragma once
#include <inttypes.h>
#include "OptionEventHandler.h"

namespace TapuinoNext
{
    enum InputResponse
    {
        None,
        Prev,
        Next,
        Select,
        Abort
    };

    class InputHandler : public IChangeHandler
    {
      public:
        InputHandler(OptionEventHandler* optionEventHandler) : optionEventHandler(optionEventHandler){};
        virtual InputResponse GetInput() = 0;

      protected:
        OptionEventHandler* optionEventHandler;
    };
} // namespace TapuinoNext
