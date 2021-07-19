#ifdef ESP32

#include "ESP32FileLoader.h"
#include "FS.h"
#include "SD.h"
#include "SD_MMC.h"
#include <stdlib.h>

using namespace TapuinoNext;

ESP32FileLoader::ESP32FileLoader()
{
}

ESP32FileLoader::~ESP32FileLoader()
{
}

bool ESP32FileLoader::Init()
{
    // needed for stable SD Card access
    pinMode(2, INPUT_PULLUP);
    return SD_MMC.begin("/sdcard", true);
}

ErrorCodes ESP32FileLoader::OpenFile(const char* fileName, File& fileOut)
{
    if (!SD_MMC.exists(fileName))
        return ErrorCodes::FILE_NOT_FOUND;

    fileOut = SD_MMC.open(fileName, "rb");
    if (!fileOut)
    {
        return ErrorCodes::FILE_ERROR;
    }
    return ErrorCodes::OK;
}

ErrorCodes ESP32FileLoader::CreateFile(const char* fileName, File& fileOut)
{
    fileOut = SD_MMC.open(fileName, "wb+");
    if (!fileOut)
    {
        return ErrorCodes::FILE_ERROR;
    }
    return ErrorCodes::OK;
}

bool ESP32FileLoader::FileExists(const char* fileName)
{
    return (SD_MMC.exists(fileName));
}

bool ESP32FileLoader::CreateDir(const char* path)
{
    return (SD_MMC.mkdir(path));
}

#endif
