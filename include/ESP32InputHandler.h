#pragma once
#ifdef ESP32

#include "InputHandler.h"
#include "Button.h"

namespace TapuinoNext
{
    class ESP32InputHandler : public InputHandler
    {
      public:
        ESP32InputHandler(OptionEventHandler* optionEventHandler);
        ~ESP32InputHandler();
        void Init();
        InputResponse GetInput();
        void OnChange(OptionTagId tag, IOptionType* option);

      private:
        void UpdateButtonTimes(uint32_t clickTime, uint32_t holdTime);
        static void ReadEncoderStatic();
        void ReadEncoder();

        static ESP32InputHandler* internalClass;
        volatile int encoderDir = 0;
        Button encoderButton;
    };
} // namespace TapuinoNext

#endif
