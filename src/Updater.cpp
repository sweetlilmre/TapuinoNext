#include "config.h"
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

void Updater::OnProgress(size_t progress, size_t size)
{
    float prog = ((float) progress / (float) size) * 100;
    char buf[I2C_DISP_COLS + 1];
    snprintf(buf, I2C_DISP_COLS + 1, "progress: %d%%", (int) prog);
    lcdUtils->Status(buf);
}

// perform the actual update from a given stream
void Updater::PerformUpdate()
{
    File updateFile;
    if (ErrorCodes::OK != fileLoader->OpenFile("/update.bin", updateFile))
    {
        lcdUtils->Error(S_UPDATE_UPDATING, S_UPDATE_NOT_FOUND);
        return;
    }

    Update.onProgress(std::bind(&Updater::OnProgress, this, std::placeholders::_1, std::placeholders::_2));

    size_t updateSize = updateFile.size();
    if (updateSize == 0)
    {
        lcdUtils->Error(S_UPDATE_UPDATING, S_UPDATE_TOO_SMALL);
    }
    else if (Update.begin(updateSize))
    {
        lcdUtils->Error(S_UPDATE_UPDATING, S_UPDATE_STARTED);
        if (Update.writeStream(updateFile) == updateSize)
        {
            lcdUtils->Error(S_UPDATE_UPDATING, S_UPDATE_WRITTEN_OK);
        }
        else
        {
            lcdUtils->Error(S_UPDATE_UPDATING, S_UPDATE_UNDERWRITE);
        }
        if (Update.end())
        {
            lcdUtils->Error(S_UPDATE_UPDATING, S_UPDATE_WRITTEN);
            if (Update.isFinished())
            {
                lcdUtils->Error(S_UPDATE_UPDATING, S_UPDATE_SUCCESS);
                lcdUtils->Error(S_UPDATE_UPDATING, S_UPDATE_REBOOT);
                delay(1000);
                ESP.restart();
                delay(1000);
            }
            else
            {
                lcdUtils->Error(S_UPDATE_UPDATING, S_UPDATE_INCOMPLETE);
            }
        }
        else
        {
            lcdUtils->Error(S_UPDATE_UPDATING, S_UPDATE_FAILED);
            Serial.println("Error Occurred. Error #: " + String(Update.getError()));
        }
    }
    else
    {
        lcdUtils->Error(S_UPDATE_UPDATING, S_UPDATE_NO_SPACE);
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
    TheMenu inPlayMenu = {S_UPDATE, (MenuEntry*) inPlayEntries, 2, 0, NULL};

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
                PerformUpdate();
                return;
            }
            // Abort (-1)
            default:
                return;
        }
    }
}
