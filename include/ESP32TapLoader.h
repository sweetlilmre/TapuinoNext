#pragma once
#ifdef ESP32

#include "TapLoader.h"
#include <Arduino.h>

namespace TapuinoNext
{
    class ESP32TapLoader : public TapLoader
    {
      public:
        ESP32TapLoader(UtilityCollection* utilityCollection, uint32_t bufferSize);
        ~ESP32TapLoader();

      protected:
        virtual void HWStartTimer();
        virtual void HWStopTimer();

        // 2Mhz timer
        // The computed signal value is for the full length of the signal as measured from high-low to high-low transition.
        // At 1 Mhz the value would need to be divided by 2 for each half, at 2 Mhz the value can be used as is
        // This changes for TAP files in half-wave format, here the length would need to be doubled, this is done in the CalcSignalTime() method
        // force inline so that this gets compiled into the ISR code with IRAM_ATTR (I hope)
        void TapSignalTimer();

      private:
        hw_timer_t* tapSignalTimer;
        static ESP32TapLoader* internalClass;
        static void IRAM_ATTR TapSignalTimerStatic();
        bool signal1stHalf = true;
        bool stopping;
        bool stopped;

        volatile uint32_t lastSignalTime = 0;
    };
} // namespace TapuinoNext
#endif
