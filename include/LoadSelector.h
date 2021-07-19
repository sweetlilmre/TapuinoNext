#pragma once

#include "Menu.h"
#include "UtilityCollection.h"

namespace TapuinoNext
{
    class LoadSelector : public ActionCallback
    {
      public:
        LoadSelector(UtilityCollection* utilityCollection);
        ~LoadSelector();
        void OnAction();

      private:
        bool Filter(File file);
        bool DisplayFileName(File dir, int index, int numFiles);
        File GetFileAtIndex(File dir, int index, int numFiles);
        int GetNumFiles(File dir);
        UtilityCollection* utilityCollection;
        LCDUtils* lcdUtils;
        InputHandler* inputHandler;
        FileLoader* fileLoader;
        Options* options;
    };
} // namespace TapuinoNext
