#pragma once
#ifdef ESP32

#include "TapRecorder.h"
#include <Arduino.h>

namespace TapuinoNext
{
    class ESP32TapRecorder : public TapRecorder
    {
      public:
        ESP32TapRecorder(UtilityCollection* utilityCollection, uint32_t bufferSize);
        ~ESP32TapRecorder();

      protected:
        void TapSignalCallback(uint64_t curEdgeTime);
        virtual void HWStartSampling();
        virtual void HWStopSampling();

      private:
        static ESP32TapRecorder* internalClass;
        static void IRAM_ATTR TapSignalCallbackStatic();
        uint64_t lastEdgeTime = 0;
    };

} // namespace TapuinoNext

#endif // ESP32