#include "TapBase.h"
#include "FS.h"
#include <HardwareSerial.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

using namespace TapuinoNext;

TapBase::TapBase(UtilityCollection* utilityCollection, uint32_t bufferSize)
{
    lcdUtils = utilityCollection->lcdUtils;
    inputHandler = utilityCollection->inputHandler;
    fileLoader = utilityCollection->fileLoader;
    options = utilityCollection->options;

    // ensure the buffer size is a power of 2
    uint64_t powerTwo = 1;
    while (powerTwo < bufferSize)
    {
        powerTwo <<= 1;
    }
    powerTwo >>= 1;

    this->bufferSize = powerTwo;
    bufferMask = this->bufferSize - 1;
    halfBufferSize = this->bufferSize >> 1;
    bufferSwitchPos = halfBufferSize;

    pBuffer = NULL;
    bufferPos = 0;
    cycleMultRaw = 1;
    cycleMult8 = 1;
    
    pinMode(C64_SENSE_PIN, OUTPUT);
    digitalWrite(C64_SENSE_PIN, HIGH);

    pinMode(C64_MOTOR_PIN, INPUT_PULLDOWN);
    //digitalWrite(C64_MOTOR_PIN, LOW);

    pinMode(C64_READ_PIN, OUTPUT);
    digitalWrite(C64_READ_PIN, LOW);

    pinMode(C64_WRITE_PIN, INPUT_PULLDOWN);
}

TapBase::~TapBase()
{
    if (pBuffer != NULL)
    {
        free(pBuffer);
        pBuffer = NULL;
    }
}

void TapBase::SetupCycleTiming()
{
    // default to C64
    double ntsc_cycles_per_second = 1022272;
    double pal_cycles_per_second = 985248;

    switch ((MACHINE_TYPE) tapInfo.platform)
    {
        case MACHINE_TYPE::C64:
            ntsc_cycles_per_second = 1022272;
            pal_cycles_per_second = 985248;
            break;
        case MACHINE_TYPE::VIC:
            ntsc_cycles_per_second = 1022727;
            pal_cycles_per_second = 1108404;
            break;
        case MACHINE_TYPE::C16:
            ntsc_cycles_per_second = 894886;
            pal_cycles_per_second = 886724;
            break;
    }

    switch ((VIDEO_MODE) tapInfo.video)
    {
        case VIDEO_MODE::PAL:
            cycleMultRaw = (1000000.0 / pal_cycles_per_second);
            break;
        case VIDEO_MODE::NSTC:
            cycleMultRaw = (1000000.0 / ntsc_cycles_per_second);
            break;
    }
    cycleMult8 = (cycleMultRaw * 8.0);
}
