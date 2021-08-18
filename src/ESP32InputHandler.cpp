#ifdef ESP32

#include <Arduino.h>
#include "ESP32InputHandler.h"
#include "config.h"

using namespace TapuinoNext;

ESP32InputHandler* ESP32InputHandler::internalClass = NULL;

ESP32InputHandler::ESP32InputHandler(OptionEventHandler* optionEventHandler) : InputHandler(optionEventHandler)
{
    internalClass = this;
    pinMode(ROTARY_ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ROTARY_ENCODER_B_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_A_PIN), ReadEncoderStatic, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_B_PIN), ReadEncoderStatic, CHANGE);
    encoderButton.Init(ROTARY_ENCODER_BUTTON_PIN);
    optionEventHandler->Subscribe(OptionTagId::ButtonClickTime, this);
    optionEventHandler->Subscribe(OptionTagId::ButtonHoldTime, this);
}

void ESP32InputHandler::OnChange(OptionTagId tag, IOptionType* option)
{
    switch (tag)
    {
        case OptionTagId::ButtonClickTime:
            encoderButton.SetClickTime(((ValueOption*) option)->GetValue());
            break;
        case OptionTagId::ButtonHoldTime:
            encoderButton.SetHoldTime(((ValueOption*) option)->GetValue());
            break;
        default:
            break;
    }
}
void ESP32InputHandler::UpdateButtonTimes(uint32_t clickTime, uint32_t holdTime)
{
    Serial.printf("clickTime: %d, holdTime: %d\n", clickTime, holdTime);
    encoderButton.SetClickTime(clickTime);
    encoderButton.SetHoldTime(holdTime);
}

void ESP32InputHandler::ReadEncoderStatic()
{
    internalClass->ReadEncoder();
}

ESP32InputHandler::~ESP32InputHandler()
{
    detachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_A_PIN));
    detachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_B_PIN));
}

/*

 CW AB state transition: 11 10 00 01
                          ^        |
                          +--------+

CCW AB state transition: 01 00 10 11
                          ^        |
                          +--------+

STATE   Direction
0000    X
0001    CW
0010    CCW
0011    X
0100    CCW
0101    X
0110    X
0111    CW
1000    CW
1001    X
1010    X
1011    CCW
1100    X
1101    CCW
1110    CW
1111    X
*/

/*
    0000    0001    0010    0011    0100    0101    0110    0111    1000    1001    1010    1011    1100    1101    1110    1111
       0,      1,      1,      0,      1,      0,      0,      1,      1,      0,      0,      1,      0,      1,      1,      0

input           pv          store
10 AB State     0010 = 1    00000010
00 AB State     1000 = 1    00101000
10 AB State     0010 = 1    10000010
00 AB State     1000 = 1    00101000
01 AB State     0001 = 1    10000001
00 AB State     0100 = 1    00010100
01 AB State     0001 = 1    01000001
11 AB State     0111 = 1    00010111 ->
01 AB State     1101 = 1    01111101
11 AB State     0111 = 1    11010111
01 AB State     1101 = 1    01111101
11 AB State     0111 = 1    11010111

10 AB State     1110 = 1    01111110
00 AB State     1000 = 1    11101000
01 AB State     0001 = 1    10000001
11 AB State     0111 = 1    00010111 ->
*/

void ESP32InputHandler::ReadEncoder()
{
    static uint8_t prevNextCode = 0;
    static uint16_t store = 0;
    static int8_t stateTable[] = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0};

    prevNextCode <<= 2;
    if (digitalRead(ROTARY_ENCODER_A_PIN))
    {
        prevNextCode |= 0b01;
    }
    if (digitalRead(ROTARY_ENCODER_B_PIN))
    {
        prevNextCode |= 0b10;
    }
    prevNextCode &= 0b00001111;

    if (stateTable[prevNextCode])
    {
        store <<= 4;
        store |= prevNextCode;
        if ((store & 0xFF) == 0b00101011)
            encoderDir = -1;
        if ((store & 0xFF) == 0b00010111)
            encoderDir = 1;
    }
}

InputResponse ESP32InputHandler::GetInput()
{
    InputResponse resp = InputResponse::None;
    if (encoderDir != 0)
    {
        if (encoderDir < 0)
        {
            resp = InputResponse::Prev;
        }
        else
        {
            resp = InputResponse::Next;
        }
        encoderDir = 0;
    }
    else
        switch (encoderButton.GetState())
        {
            case ButtonState::CLICKED:
                resp = InputResponse::Select;
                break;
            case ButtonState::LONG_CLICKED:
                resp = InputResponse::Abort;
                break;
            default:
                break;
        }
    return resp;
}

#endif
