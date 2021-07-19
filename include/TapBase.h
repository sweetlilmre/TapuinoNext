#pragma once

#include "ErrorCodes.h"
#include "SharedTypes.h"
#include "UtilityCollection.h"
#include "config.h"

namespace TapuinoNext
{

#define TAP_HEADER_LENGTH 20
#define TAP_HEADER_MAGIC_LENGTH 12
#define TAP_HEADER_DATA_LENGTH 8

    struct TAP_INFO
    {
        uint8_t version;  // TAP file format version:
                          // Version  0: 8-bit data, 0x00 indicates overflow
                          //          1: 8-bit, 0x00 indicates 24-bit overflow to follow
                          //          2: same as 1 but with 2 half-wave values
        uint8_t platform; // Platform 0: C64
                          //          1: VIC
                          //          2: C16
        uint8_t video;    // Video    0: PAL
                          //          1: NTSC
        uint8_t reserved;
        uint32_t length;   // total length of the TAP data excluding header in bytes
        uint32_t position; // current byte position in the file
        uint32_t cycles;   //
        uint16_t counterActual;
        uint16_t counterVisual;
    };

    // as per formula and constants in vice emulator header file datasette.h
    //
    /* Counter is c=g*(sqrt(v*t/d*pi+r^2/d^2)-r/d) */

#ifndef PI
#define PI 3.1415926535
#endif
#define DS_D 1.27e-5
#define DS_R 1.07e-2
#define DS_V_PLAY 4.76e-2
#define DS_G 0.525

// magic strings found in the TAP header, represented as uint32_t values in little endian format (reversed string)
#define TAP_MAGIC_C64 0x2D343643      // "C64-"  as "-46C"
#define TAP_MAGIC_C16 0x2D363143      // "C16-"  as "-61C"
#define TAP_MAGIC_POSTFIX1 0x45504154 // "TAPE"  as "EPAT"
#define TAP_MAGIC_POSTFIX2 0x5741522D // "-RAW"  as "WAR-"

#define CYCLES_TO_COUNTER(cycles) (uint16_t)(DS_G * (sqrt((cycles / 1000000.0 * (DS_V_PLAY / DS_D / PI)) + ((DS_R * DS_R) / (DS_D * DS_D))) - (DS_R / DS_D)));

    class TapBase
    {
      public:
        TapBase(UtilityCollection* utilityCollection, uint32_t bufferSize);
        virtual ~TapBase();
        const TAP_INFO* GetTapInfo()
        {
            return &tapInfo;
        }

      protected:
        /******************************************************/
        TAP_INFO tapInfo;
        volatile bool processSignal = false; // flag to indicate weather the timer should process the TAP data or not (used for EOF conditions)
        volatile bool motorOn = false;       // flag indicating weather the C64 datasette motor is ON or OFF
        /******************************************************/

        void SetupCycleTiming();

        FileLoader* fileLoader;
        InputHandler* inputHandler;
        LCDUtils* lcdUtils;
        Options* options;

        uint8_t* pBuffer;                       // buffer of raw TAP data
        uint32_t bufferSize;                    // full size of the TAP buffer this is a ^2
        uint32_t halfBufferSize;                // half size of the TAP buffer
        uint32_t bufferPos;                     // current position in the TAP buffer
        uint32_t bufferMask;                    // bit mask used to reset the bufferPos as it moves passed the bufferSize
        uint32_t bufferSwitchPos;               // position at which to switch to the "next" half of the buffer, alternates between halfBufferSize and 0
        volatile bool bufferSwitchFlag = false; // flag set by ReadByte that notifies the main loop to fill the next half of the buffer

        // cycle multipliers are PAL / NSTC and machine dependent and calculated at run time
        // these values convert the TAP data into the number of microseconds that the signal last for (high and low edge)
        double cycleMultRaw; // cycle multiplier for a raw (3 byte value from a TAP file, version 1+)
        double cycleMult8;   // cycle multiplier for a scaled (1 byte value from a TAP file)
    };
} // namespace TapuinoNext
