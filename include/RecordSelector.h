#pragma once

#include "Menu.h"
#include "UtilityCollection.h"

namespace TapuinoNext
{
    class RecordSelector : public ActionCallback
    {
      public:
        RecordSelector(UtilityCollection* utilityCollection);
        ~RecordSelector();
        void OnAction();
        ErrorCodes ManualFilename(File& recTap);
        ErrorCodes AutoFileName(File& recTap);
        bool ManualAutoMenu(File& tapFile);

      private:
        UtilityCollection* utilityCollection;
        LCDUtils* lcdUtils;
        InputHandler* inputHandler;
        FileLoader* fileLoader;
        Options* options;
    };
} // namespace TapuinoNext
