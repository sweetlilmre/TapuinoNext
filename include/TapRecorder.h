#pragma once

#include "ErrorCodes.h"
#include "FileLoader.h"
#include "InputHandler.h"
#include "LCDUtils.h"
#include "TapBase.h"
#include "config.h"

namespace TapuinoNext
{
    class TapRecorder : public TapBase
    {
      public:
        TapRecorder(UtilityCollection* utilityCollection, uint32_t bufferSize);
        ~TapRecorder();
        void RecordTap(File tapFile);

      protected:
        // Interface to the hardware implementing derrived class
        /******************************************************/
        virtual void HWStartSampling() = 0;
        virtual void HWStopSampling() = 0;
        void CalcTapData(uint32_t signalTime);
        /******************************************************/

      private:
        bool InRecordMenu(File tapFile);
        ErrorCodes CreateTap(File tapFile);
        void StartSampling();
        void StopSampling();

        void FlushBufferIfNeeded(File tapFile);
        void FlushBufferFinal(File tapFile);
        void WriteByte(uint8_t value);
    };
} // namespace TapuinoNext
