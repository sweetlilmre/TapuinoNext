#pragma once
#include "InputHandler.h"
#include "LCDUtils.h"
#include "MenuTypes.h"
#include "FileLoader.h"

namespace TapuinoNext
{
    class Updater : public ActionCallback
    {
      public:
        Updater(LCDUtils* lcdUtils, InputHandler* inputHandler, FileLoader* fileLoader);
        void OnAction();

      private:
        LCDUtils* lcdUtils;
        InputHandler* inputHandler;
        FileLoader* fileLoader;

        void PerformUpdate();
        void OnProgress(size_t progress, size_t size);
    };
} // namespace TapuinoNext