#ifdef ESP32
#include "ESP32TapLoader.h"

using namespace TapuinoNext;

ESP32TapLoader* ESP32TapLoader::internalClass = NULL;

ESP32TapLoader::ESP32TapLoader(UtilityCollection* utilityCollection, uint32_t bufferSize) : TapLoader(utilityCollection, bufferSize)
{
    ESP32TapLoader::internalClass = this;
    tapSignalTimer = NULL;
    stopping = false;
    stopped = false;
}

ESP32TapLoader::~ESP32TapLoader()
{
}

#define IDLE_TIMER_EXECUTE 1000

void ESP32TapLoader::HWStartTimer()
{
    Serial.printf("HWStartTimer: Stopping: %d, Stopped: %d\n", stopping, stopped);
    if (tapSignalTimer == NULL)
    {
        stopping = false;
        stopped = false;

        // set up 2Mhz timer
        tapSignalTimer = timerBegin(0, 40, true);
        timerAttachInterrupt(tapSignalTimer, &ESP32TapLoader::TapSignalTimerStatic, true);
        timerWrite(tapSignalTimer, 0);
        timerAlarmWrite(tapSignalTimer, IDLE_TIMER_EXECUTE, true);
        timerAlarmEnable(tapSignalTimer);
    }
    else
    {
        Serial.printf("timer WAS NOT was null!\n");
    }
}

void ESP32TapLoader::HWStopTimer()
{
    Serial.printf("HWStopTimer: Stopping: %d, Stopped: %d\n", stopping, stopped);
    if (tapSignalTimer != NULL)
    {
        stopping = true;
        while (!stopped)
        {
            Serial.println("waiting for: stopped");
            delay(10);
        };
        timerAlarmDisable(tapSignalTimer);
        timerDetachInterrupt(tapSignalTimer);
        timerEnd(tapSignalTimer);
        tapSignalTimer = NULL;
        stopping = false;
        stopped = false;
    }
    else
    {
        Serial.printf("timer WAS null!\n");
    }
}

void IRAM_ATTR ESP32TapLoader::TapSignalTimerStatic()
{
    ESP32TapLoader::internalClass->TapSignalTimer();
}

void IRAM_ATTR ESP32TapLoader::TapSignalTimer()
{
    // default to an idle mode that keeps the timer ticking while not processing any signals
    uint32_t signalTime = IDLE_TIMER_EXECUTE;

    if (processSignal && motorOn)
    {
        if (signal1stHalf)
        {
            tapInfo.cycles += (lastSignalTime >> 1);
            lastSignalTime = signalTime = CalcSignalTime();
            // special marker indicating that the end of the TAP has been reached.
            if (lastSignalTime == 0xFFFFFFFF)
            {
                processSignal = false;
                stopped = true;
                return;
            }
            // half-wave format, point to the next signal for the 2nd half value
            if (tapInfo.version == 2)
            {
                lastSignalTime = CalcSignalTime();
            }
            digitalWrite(C64_READ_PIN, LOW);
        }
        else
        {
            tapInfo.cycles += (lastSignalTime >> 1);
            signalTime = lastSignalTime;
            digitalWrite(C64_READ_PIN, HIGH);
        }

        signal1stHalf = !signal1stHalf;
    }

    if (!stopping)
    {
        timerAlarmWrite(tapSignalTimer, signalTime, true);
    }
    else
    {
        stopped = true;
    }
}

#endif
