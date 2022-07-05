#pragma once

#include "Menu.h"
#include "UtilityCollection.h"

enum class FILE_TYPE : uint8_t
{
    INVALID = 0,
    TAP = 1,
    PRG = 2
};

namespace TapuinoNext
{
    class LoadSelector : public ActionCallback
    {
      public:
        LoadSelector(UtilityCollection* utilityCollection);
        ~LoadSelector();
        void OnAction();

      private:
        FILE_TYPE GetExtensionType(File file);
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
