#pragma once

#include "ErrorCodes.h"
#include "TapBase.h"

namespace TapuinoNext
{
    class TapLoader : public TapBase
    {
      public:
        TapLoader(UtilityCollection* utilityCollection, uint32_t bufferSize);
        ~TapLoader();
        void PlayTap(File tapFile);
        const TAP_INFO* GetTapInfo()
        {
            return &tapInfo;
        }

      protected:
        // Interface to the hardware implementing derrived class
        /******************************************************/
        virtual void HWStartTimer() = 0;
        virtual void HWStopTimer() = 0;
        /******************************************************/
        uint32_t CalcSignalTime();

      private:
        bool isTiming;
        uint32_t ReadNextByte();
        ErrorCodes VerifyTap(File tapFile);
        void StartTimer();
        void StopTimer();

        bool InPlayMenu(File tapFile);
        bool SeekToCounter(File tapFile, uint16_t targetCounter);
    };
} // namespace TapuinoNext
