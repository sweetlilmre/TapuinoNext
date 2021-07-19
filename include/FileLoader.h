#pragma once

#include <inttypes.h>
#include "ErrorCodes.h"
#include "FS.h"

namespace TapuinoNext
{
    class FileLoader
    {
      public:
        FileLoader(){};
        ~FileLoader(){};
        virtual bool Init() = 0;
        virtual ErrorCodes OpenFile(const char* fileName, File& fileOut) = 0;
        virtual ErrorCodes CreateFile(const char* fileName, File& fileOut) = 0;
        virtual bool FileExists(const char* fileName) = 0;
        virtual bool CreateDir(const char* fileName) = 0;
    };
} // namespace TapuinoNext
