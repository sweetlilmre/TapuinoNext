#include "ESP32TapRecorder.h"

using namespace TapuinoNext;

ESP32TapRecorder* ESP32TapRecorder::internalClass = NULL;

ESP32TapRecorder::ESP32TapRecorder(UtilityCollection* utilityCollection) : TapRecorder(utilityCollection)
{
    ESP32TapRecorder::internalClass = this;
}

ESP32TapRecorder::~ESP32TapRecorder()
{
}

void ESP32TapRecorder::TapSignalCallback(uint64_t curEdgeTime)
{
    if (lastEdgeTime == 0)
    {
        lastEdgeTime = curEdgeTime;
    }
    else
    {
        uint64_t diffTime = curEdgeTime - lastEdgeTime;
        lastEdgeTime = curEdgeTime;
        CalcTapData(diffTime);
    }
}

void ESP32TapRecorder::HWStartSampling()
{
    lastEdgeTime = 0;
    attachInterrupt(digitalPinToInterrupt(C64_WRITE_PIN), TapSignalCallbackStatic, RISING);
}

void ESP32TapRecorder::HWStopSampling()
{
    detachInterrupt(digitalPinToInterrupt(C64_WRITE_PIN));
}

void IRAM_ATTR ESP32TapRecorder::TapSignalCallbackStatic()
{
    // capture as close to the interrupt as possible
    uint64_t curEdgeTime = esp_timer_get_time();
    ESP32TapRecorder::internalClass->TapSignalCallback(curEdgeTime);
}
