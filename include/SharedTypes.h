#pragma once
#include <inttypes.h>

namespace TapuinoNext
{
    extern const char* machineTypeNames[];

    /*
    TODO: Vice has definitions for other machine types
    PET      3
    C500     4
    C600     5
    */

    enum class MACHINE_TYPE : uint8_t
    {
        C64 = 0,
        VIC = 1,
        C16 = 2
    };

    extern const char* videoModeNames[];

    /*
    TODO: Vice has definitions for other video types

    NTSCOLD   2
    PALN      3
    */

    enum class VIDEO_MODE : uint8_t
    {
        PAL = 0,
        NSTC = 1
    };

} // namespace TapuinoNext