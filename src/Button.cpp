#include <Arduino.h>
#include "Button.h"

using namespace TapuinoNext;

Button::Button()
{
}

Button::Button(uint8_t pin, uint16_t pressTime, uint16_t releaseTime, uint16_t holdTime)
{
    Init(pin, pressTime, releaseTime, holdTime);
}

void Button::Init(uint8_t pin, uint16_t pressTime, uint16_t releaseTime, uint16_t holdTime)
{
    this->pin = pin;
    this->pressTime = pressTime;
    this->releaseTime = releaseTime;
    this->holdTime = holdTime;
    state = PinState::NONE;
    held = false;
    pinMode(pin, INPUT_PULLUP);
}

Button::~Button()
{
}

void Button::SetClickTime(uint32_t clickTime)
{
    this->pressTime = clickTime;
    this->releaseTime = clickTime;
}

void Button::SetHoldTime(uint32_t holdTime)
{
    this->holdTime = holdTime;
}

ButtonState Button::GetState()
{
    uint8_t buttonPressed = !digitalRead(pin);
    switch (state)
    {
        case PinState::NONE:
        {
            if (buttonPressed)
            {
                pressedStart = millis();
                state = PinState::IN_PRESS;
            }
            break;
        }
        case PinState::IN_PRESS:
        {
            if (!buttonPressed)
            {
                if (millis() - pressedStart > pressTime)
                {
                    state = PinState::IN_RELEASE;
                    releaseStart = millis();
                }
            }
            else if (millis() - pressedStart > holdTime)
            {
                held = true;
            }
            break;
        }
        case PinState::IN_RELEASE:
        {
            if (millis() - releaseStart > releaseTime)
            {
                state = PinState::NONE;
                if (held)
                {
                    held = false;
                    return (ButtonState::LONG_CLICKED);
                }
                return (ButtonState::CLICKED);
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return (ButtonState::NONE);
}
