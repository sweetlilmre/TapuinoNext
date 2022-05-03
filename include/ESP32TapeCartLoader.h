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
        bool CheckForMode();

      protected:
        void MotorSignalCallback(bool writeHigh);
        virtual void HWStartSampling();
        virtual void HWStopSampling();

      private:
        void FastSendByte(uint8_t byte);
        UtilityCollection* utilityCollection;
        static ESP32TapeCartLoader* internalClass;
        static void IRAM_ATTR MotorSignalCallbackStatic();
        volatile uint16_t shiftReg;
        bool loaderMode;
    };
} // namespace TapuinoNext
#endif
