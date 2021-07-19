#pragma once
#include "FileLoader.h"
#include "InputHandler.h"
#include "LCDUtils.h"
#include "Options.h"

namespace TapuinoNext
{
    class UtilityCollection
    {
      public:
        UtilityCollection(LCDUtils* lcdUtils, InputHandler* inputHandler, FileLoader* fileLoader, Options* options)
            : lcdUtils(lcdUtils), inputHandler(inputHandler), fileLoader(fileLoader), options(options)
        {
        }

        ~UtilityCollection()
        {
        }

        LCDUtils* lcdUtils;
        InputHandler* inputHandler;
        FileLoader* fileLoader;
        Options* options;
    };
} // namespace TapuinoNext