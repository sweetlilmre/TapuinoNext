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
        TapRecorder(UtilityCollection* utilityCollection);
        ~TapRecorder();
        void RecordTap(File tapFile);

      protected:
        // Interface for the derrived class that implemtents the hardware interface
        /******************************************************/
        virtual void HWStartSampling() = 0;
        virtual void HWStopSampling() = 0;
        /******************************************************/
        void CalcTapData(uint32_t signalTime);
        void FinalizeRecording(File tapFile);

      private:
        bool isSampling;
        void WriteNextByte(uint8_t nextByte);
        bool InRecordMenu(File tapFile);
        ErrorCodes CreateTap(File tapFile);
        void StartSampling();
        void StopSampling();
    };
} // namespace TapuinoNext
