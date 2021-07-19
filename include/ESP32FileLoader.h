#pragma once

#ifdef ESP32

#include "FileLoader.h"

namespace TapuinoNext
{
    class ESP32FileLoader : public FileLoader
    {
      public:
        ESP32FileLoader();
        ~ESP32FileLoader();
        virtual bool Init();
        virtual ErrorCodes OpenFile(const char* fileName, File& fileOut);
        virtual ErrorCodes CreateFile(const char* fileName, File& fileOut);
        virtual bool FileExists(const char* fileName);
        virtual bool CreateDir(const char* fileName);
    };
} // namespace TapuinoNext

#endif
