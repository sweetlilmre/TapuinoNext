#pragma once
#include <inttypes.h>

namespace TapuinoNext
{
    enum class ErrorCodes : uint8_t
    {
        OK = 0,
        FILE_NOT_FOUND,
        INVALID_TAP_FILE,
        UNKNOWN_TAP_FORMAT,
        OUT_OF_MEMORY,
        FILE_ERROR,
        INVALID_COUNTER_POS,
        OUT_OF_RANGE,
        OPERATION_ABORTED
    };
} // namespace TapuinoNext