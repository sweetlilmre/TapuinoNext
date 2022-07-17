#pragma once
#include <Arduino.h>
#include "FS.h"
#include "ErrorCodes.h"

namespace TapuinoNext
{
    class FlipBuffer
    {
      public:
        FlipBuffer(uint32_t bufferSize);
        ~FlipBuffer();
        virtual ErrorCodes Init();
        void Reset();

        ErrorCodes SetHeader(uint8_t* byteFiller, uint32_t size);
        ErrorCodes FillWholeBuffer(File file, uint32_t atPos = 0);

        uint8_t ReadByte();
        void FillBufferIfNeeded(File tapFile);

        void WriteByte(uint8_t value);
        void FlushBufferIfNeeded(File tapFile);
        void FlushBufferFinal(File tapFile);


      private:
        uint8_t* pBuffer;                       // buffer of raw TAP data
        uint32_t bufferSize;                    // full size of the TAP buffer this is a ^2
        uint32_t halfBufferSize;                // half size of the TAP buffer
        uint32_t bufferPos;                     // current position in the TAP buffer
        uint32_t bufferMask;                    // bit mask used to reset the bufferPos as it moves passed the bufferSize
        uint32_t bufferSwitchPos;               // position at which to switch to the "next" half of the buffer, alternates between halfBufferSize and 0
        volatile bool bufferSwitchFlag = false; // flag set by ReadByte that notifies the main loop to fill the next half of the buffer
    };
} // namespace TapuinoNext
