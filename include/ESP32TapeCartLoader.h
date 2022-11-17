#pragma once
#ifdef ESP32

#include <Arduino.h>
#include "UtilityCollection.h"
#include "config.h"

namespace TapuinoNext
{
    class ESP32TapeCartLoader
    {
      public:
        ESP32TapeCartLoader(UtilityCollection* utilityCollection);
        ~ESP32TapeCartLoader();
        void Init();
        bool LoadPRG(File& prgFile);

      protected:
        void MotorSignalCallback(int writeHigh);
        virtual void HWStartSampling();
        virtual void HWStopSampling();

      private:
        bool CheckForMode();
        bool FastSendByte(uint8_t byte);
        bool FastSendByteInner(uint8_t byte);
        UtilityCollection* utilityCollection;
        FlipBuffer* flipBuffer;
        static ESP32TapeCartLoader* internalClass;
        static void IRAM_ATTR MotorSignalCallbackStatic();
        volatile uint16_t shiftReg;
        bool loaderMode;
        File prgFile;
    };
} // namespace TapuinoNext
#endif
