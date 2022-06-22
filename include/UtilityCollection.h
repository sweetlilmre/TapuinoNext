#pragma once
#include "FileLoader.h"
#include "InputHandler.h"
#include "LCDUtils.h"
#include "Options.h"
#include "FlipBuffer.h"

namespace TapuinoNext
{
    class UtilityCollection
    {
      public:
        UtilityCollection(LCDUtils* lcdUtils, InputHandler* inputHandler, FileLoader* fileLoader, Options* options, FlipBuffer* flipBuffer)
            : lcdUtils(lcdUtils), inputHandler(inputHandler), fileLoader(fileLoader), options(options), flipBuffer(flipBuffer)
        {
        }

        ~UtilityCollection()
        {
        }

        LCDUtils* lcdUtils;
        InputHandler* inputHandler;
        FileLoader* fileLoader;
        Options* options;
        FlipBuffer* flipBuffer;
    };
} // namespace TapuinoNext