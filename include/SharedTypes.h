#pragma once
#include <inttypes.h>

namespace TapuinoNext
{
    extern const char* machineTypeNames[];

    enum class MACHINE_TYPE : uint8_t
    {
        C64,
        VIC,
        C16
    };

    extern const char* videoModeNames[];

    enum class VIDEO_MODE : uint8_t
    {
        PAL,
        NSTC
    };

} // namespace TapuinoNext