#include "LoadSelector.h"
#include "ESP32TapLoader.h"
// #include "ESP32TapeCartLoader.h"
#include "FS.h"
#include "Lang.h"

using namespace TapuinoNext;

LoadSelector::LoadSelector(UtilityCollection* utilityCollection) : utilityCollection(utilityCollection)
{
    lcdUtils = utilityCollection->lcdUtils;
    inputHandler = utilityCollection->inputHandler;
    fileLoader = utilityCollection->fileLoader;
    options = utilityCollection->options;
}

LoadSelector::~LoadSelector()
{
}

bool LoadSelector::Filter(File file)
{
// ".tap"
#define TAP_MAGIC_BIG_ENDIAN 0x2E746170
// "pat."
#define TAP_MAGIC_LITTLE_ENDIAN 0x7061742E

    // no files or directories with one "." as first char
    const char* name = file.name();
    if (name[0] == '.')
    {
        if (name[1] == '.')
        {
            return (true);
        }
        else
        {
            return (false);
        }
    }

    if (file.isDirectory())
    {
        return (true);
    }

    const char* ext = strrchr(file.name(), '.');
    if (ext)
    {
        // capture the extension as a mutable 32-bit number (turn .tap into a 32-bit number trick)
        uint32_t magic = *(uint32_t*) ext;
        // and then turn magic into an 8-bit array
        uint8_t* ch = (uint8_t*) &magic;
        // lower case the 'string'
        for (int i = 0; i < 4; i++)
        {
            if (ch[i] >= 'A' && ch[i] <= 'Z')
            {
                ch[i] += 32;
            }
        }
        // ESP32 is little endian, but the code is written to be as portable as possible
        // could optimise with a once off endian check some where
        if (magic == TAP_MAGIC_LITTLE_ENDIAN || magic == TAP_MAGIC_BIG_ENDIAN)
        {
            return (true);
        }
    }
    return (false);
}

File LoadSelector::GetFileAtIndex(File dir, int index, int numFiles)
{
    dir.rewindDirectory();
    File curFile;
    int i = 0;
    while (i < numFiles)
    {
        curFile = dir.openNextFile();
        if (Filter(curFile))
        {
            if (i == index)
            {
                return (curFile);
            }
            i++;
        }
    }
    // and here is the classic:
    // This should be impossible!
    lcdUtils->Error("Fatal Error!", "Halting");
    while (true)
    {
    }
}

int LoadSelector::GetNumFiles(File dir)
{
    int numFiles = 0;
    File curFile;
    while (curFile = dir.openNextFile())
    {
        if (Filter(curFile))
        {
            numFiles++;
        }

        curFile.close();
    }
    return (numFiles);
}

bool LoadSelector::DisplayFileName(File dir, int index, int numFiles)
{
    bool isDirectory = true;
    if (index == 0)
    {
        lcdUtils->ShowFile("..", false, true);
    }
    else
    {
        File f = GetFileAtIndex(dir, index - 1, numFiles);
        isDirectory = f.isDirectory();
        lcdUtils->ShowFile(f.name(), false, isDirectory);
        f.close();
    }
    return (isDirectory);
}

void LoadSelector::OnAction()
{
    File dir;
    ErrorCodes err = fileLoader->OpenFile("/", dir);
    if (err != ErrorCodes::OK)
    {
        lcdUtils->Error(S_FILE_ERROR, err);
        return;
    }

    if (!dir.isDirectory())
    {
        lcdUtils->Error(S_FILE_ERROR, "");
        return;
    }

    lcdUtils->ShowFile(dir.name(), true);
    int numFiles = GetNumFiles(dir);

    if (numFiles == 0)
    {
        lcdUtils->Error(S_NO_FILES_FOUND, "");
        return;
    }

    int curIndex = 0;
    bool isDirectory = DisplayFileName(dir, curIndex, numFiles);
    uint32_t tickerTime = options->tickerTime.GetValue();
    uint32_t tickerHoldTime = options->tickerHoldTime.GetValue();

    while (true)
    {
        lcdUtils->ScrollFileName(tickerTime, tickerHoldTime, isDirectory);
        switch (inputHandler->GetInput())
        {
            case InputResponse::Next:
                curIndex++;
                if (curIndex > numFiles)
                {
                    curIndex = 0;
                }
                isDirectory = DisplayFileName(dir, curIndex, numFiles);
                break;

            case InputResponse::Prev:
                curIndex--;
                if (curIndex < 0)
                {
                    curIndex = numFiles;
                }
                isDirectory = DisplayFileName(dir, curIndex, numFiles);
                break;

            case InputResponse::Select:
                if (isDirectory)
                {
                    if (curIndex == 0)
                    {
                        const char* curDirName = dir.path();
                        char* prevPath = strrchr(curDirName, '/');
                        if (!prevPath)
                            break;

                        File tmpFile;
                        ErrorCodes err;

                        if (prevPath == curDirName)
                        {
                            // going back to the root.
                            err = fileLoader->OpenFile("/", tmpFile);
                        }
                        else
                        {
                            char ch = *prevPath;
                            *prevPath = 0;
                            err = fileLoader->OpenFile(curDirName, tmpFile);
                            *prevPath = ch;
                        }

                        dir.close();

                        if (err != ErrorCodes::OK)
                        {
                            lcdUtils->Error(S_FILE_ERROR, err);
                            return;
                        }
                        dir = tmpFile;
                    }
                    else
                    {
                        File tmpFile = GetFileAtIndex(dir, curIndex - 1, numFiles);
                        dir.close();
                        dir = tmpFile;
                    }

                    lcdUtils->ShowFile(dir.name(), true);
                    numFiles = GetNumFiles(dir);
                    curIndex = 0;
                    isDirectory = DisplayFileName(dir, curIndex, numFiles);
                }
                else
                {
                    ESP32TapLoader tl(utilityCollection);
                    // ESP32TapeCartLoader tapeCartLoader(utilityCollection);
                    // tapeCartLoader.Init();
                    File tmpFile = GetFileAtIndex(dir, curIndex - 1, numFiles);

                    tl.PlayTap(tmpFile);
                    // tapeCartLoader.CheckForMode();
                    tmpFile.close();
                    dir.close();
                    return;
                }
                break;
            case InputResponse::Abort:
                return;
                break;
            default:
                break;
        }
    }
}
