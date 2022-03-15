#include "Updater.h"
#include "Menu.h"
#include "Lang.h"
#include "FileLoader.h"
#include "ErrorCodes.h"
#include <Update.h>
#include <FS.h>

using namespace TapuinoNext;

Updater::Updater(LCDUtils* lcdUtils, InputHandler* inputHandler, FileLoader* fileLoader)
{
    this->lcdUtils = lcdUtils;
    this->inputHandler = inputHandler;
    this->fileLoader = fileLoader;
}

// perform the actual update from a given stream
void Updater::PerformUpdate()
{
    File updateFile;
    if (ErrorCodes::OK != fileLoader->OpenFile("/update.bin", updateFile))
    {
        lcdUtils->Error(S_UPDATE_MENU, S_UPDATE_NOT_FOUND);
        return;
    }

    size_t updateSize = updateFile.size();
    if (updateSize == 0)
    {
        lcdUtils->Error(S_UPDATE_MENU, S_UPDATE_TOO_SMALL);
    }
    else if (Update.begin(updateSize))
    {
        if (Update.writeStream(updateFile) == updateSize)
        {
            lcdUtils->Error(S_UPDATE_MENU, S_UPDATE_WRITTEN_OK);
        }
        else
        {
            lcdUtils->Error(S_UPDATE_MENU, S_UPDATE_UNDERWRITE);
        }
        if (Update.end())
        {
            lcdUtils->Error(S_UPDATE_MENU, S_UPDATE_WRITTEN);
            if (Update.isFinished())
            {
                lcdUtils->Error(S_UPDATE_MENU, S_UPDATE_SUCCESS);
                lcdUtils->Error(S_UPDATE_MENU, S_UPDATE_REBOOT);
            }
            else
            {
                lcdUtils->Error(S_UPDATE_MENU, S_UPDATE_INCOMPLETE);
            }
        }
        else
        {
            lcdUtils->Error(S_UPDATE_MENU, S_UPDATE_FAILED);
            Serial.println("Error Occurred. Error #: " + String(Update.getError()));
        }
    }
    else
    {
        lcdUtils->Error(S_UPDATE_MENU, S_NO_UPDATE_SPACE);
    }
    updateFile.close();
}

void Updater::OnAction()
{
    MenuHandler menu(lcdUtils, inputHandler);

    MenuEntry inPlayEntries[] = {
        {MenuEntryType::IndexEntry, S_EXIT, NULL},
        {MenuEntryType::IndexEntry, S_UPDATE, NULL},
    };
    TheMenu inPlayMenu = {S_UPDATE_MENU, (MenuEntry*) inPlayEntries, 3, 0, NULL};

    while (true)
    {
        switch (menu.Display(&inPlayMenu))
        {
            // Exit
            case 0:
            {
                return;
            }
            // Seek
            case 1:
            {
            }
            // Abort (-1)
            default:
                return;
        }
    }
}
