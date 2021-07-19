#pragma once

namespace TapuinoNext
{
    enum ButtonState
    {
        NONE,
        CLICKED,
        LONG_CLICKED,
    };

    enum class PinState : uint8_t
    {
        NONE,
        IN_PRESS,
        IN_RELEASE,
        IN_HOLD,
        RELEASED,
    };

    class Button
    {
      public:
        Button();
        Button(uint8_t pin, uint16_t pressTime = 100, uint16_t releaseTime = 100, uint16_t holdTime = 500);
        void Init(uint8_t pin, uint16_t pressTime = 100, uint16_t releaseTime = 100, uint16_t holdTime = 500);
        ~Button();
        ButtonState GetState();
        void SetClickTime(uint32_t clickTime);
        void SetHoldTime(uint32_t holdTime);

      private:
        uint8_t pin;
        PinState state;
        uint16_t pressTime, releaseTime, holdTime;
        bool held;
        unsigned long pressedStart = 0;
        unsigned long releaseStart = 0;
    };
} // namespace TapuinoNext
