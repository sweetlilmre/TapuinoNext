#pragma once

#include "OptionTypes.h"
#include <vector>

namespace TapuinoNext
{
    class IChangeHandler
    {
      public:
        virtual void OnChange(OptionTagId tag, IOptionType* option) = 0;
    };

    class OptionEventHandler : public IChangeNotify
    {
      public:
        OptionEventHandler();
        ~OptionEventHandler();

        virtual void Notify(OptionTagId tag, IOptionType* optionType);
        void Subscribe(OptionTagId tag, IChangeHandler* handler);

      private:
        std::vector<IChangeHandler*> handlers[(uint8_t) OptionTagId::LAST];
    };

} // namespace TapuinoNext
