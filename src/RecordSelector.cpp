#include "RecordSelector.h"
#include "ESP32TapRecorder.h"
#include "FS.h"
#include "LCD.h"
#include "Lang.h"

using namespace TapuinoNext;

RecordSelector::RecordSelector(UtilityCollection* utilityCollection) : utilityCollection(utilityCollection)
{
    lcdUtils = utilityCollection->lcdUtils;
    inputHandler = utilityCollection->inputHandler;
    fileLoader = utilityCollection->fileLoader;
    options = utilityCollection->options;
}

RecordSelector::~RecordSelector()
{
}

ErrorCodes RecordSelector::ManualFilename(File& recTap)
{
    // max filename length is: sub directory ("/recorded/") + displayable space + extension (".tap") + 1 for null terminator
#define MAX_FILENAME_LEN 10 + I2C_DISP_COLS + 4 + 1
    enum class CursorMode : uint8_t
    {
        Move,
        SelectChar
    };

    const char alphabet[] = " abcdefghijklmnopqrstuvwxyz0123456789_-";
    uint8_t selCharPos = 0;
    uint8_t cursorPos = 0;
    uint8_t maxChars = strlen(alphabet);
    uint8_t curChar = 0;

    LCD* lcd = lcdUtils->GetLCD();
    // start with a nicely terminated string!
    char fileNameBuffer[MAX_FILENAME_LEN] = {0};

    lcdUtils->Title(S_ENTER_FILENAME);
    lcdUtils->Status("", ENTER_INDICATOR);
    lcd->Cursor();
    lcd->SetCursor(0, 1);
    bool cursorOn = true;

    uint32_t lastTick = 0;
    uint32_t curTick = millis();
    uint32_t tickerTime = options->tickerTime.GetValue();

    CursorMode mode = CursorMode::Move;
    memset(fileNameBuffer, 32, MAX_FILENAME_LEN - 1);
    fileNameBuffer[I2C_DISP_COLS] = 0;

    while (true)
    {
        switch (mode)
        {
            case CursorMode::SelectChar:
            {
                curTick = millis();
                if (curTick - lastTick > tickerTime)
                {
                    lastTick = curTick;
                    cursorOn = !cursorOn;
                    cursorOn ? lcd->Cursor() : lcd->NoCursor();
                }
                break;
            }

            case CursorMode::Move:
            {
                if (!cursorOn)
                {
                    cursorOn = true;
                    lcd->Cursor();
                    lcd->SetCursor(cursorPos, 1);
                }
                break;
            }
        }

        switch (inputHandler->GetInput())
        {
            case InputResponse::Select:
            {
                // enter character position
                if (cursorPos == I2C_DISP_COLS - 1)
                {
                    String s = String(fileNameBuffer);
                    s.trim();
                    if (s.length() == 0)
                    {
                        lcdUtils->Error(S_INVALID_NAME, "");
                        lcdUtils->Title(S_ENTER_FILENAME);
                        lcdUtils->Status(fileNameBuffer, ENTER_INDICATOR);
                        lcd->SetCursor(cursorPos, 1);
                        continue;
                    }
                    else
                    {
                        strcpy(fileNameBuffer, "/recorded/");
                        strcat(fileNameBuffer, s.c_str());
                        strcat(fileNameBuffer, ".tap");
                        Serial.printf("Recording to manually named file: %s\n", fileNameBuffer);
                        if (!fileLoader->FileExists(fileNameBuffer))
                        {
                            ErrorCodes ret = fileLoader->CreateFile(fileNameBuffer, recTap);
                            Serial.printf("Creating recording at: %s: ErrorCodes: %d\n", fileNameBuffer, (int) ret);
                            if (ret != ErrorCodes::OK)
                            {
                                lcdUtils->Error(S_RECORDING_FAIL, ret);
                            }
                            return (ret);
                        }
                        lcdUtils->Error(S_RECORDING_FAIL, ErrorCodes::FILE_EXISTS_ERROR);
                        return (ErrorCodes::FILE_ERROR);
                    }
                }
                else if (cursorPos < (I2C_DISP_COLS - 1))
                {
                    switch (mode)
                    {
                        case CursorMode::Move:
                        {
                            mode = CursorMode::SelectChar;
                            selCharPos = 0;
                            curChar = fileNameBuffer[cursorPos];
                            while (alphabet[selCharPos] != curChar)
                            {
                                selCharPos++;
                            }
                            break;
                        }
                        case CursorMode::SelectChar:
                        {
                            mode = CursorMode::Move;
                            lcd->SetCursor(cursorPos, 1);
                            break;
                        }
                    }
                }
                break;
            }

            case InputResponse::Abort:
            {
                return ErrorCodes::OPERATION_ABORTED;
                break;
            }

            case InputResponse::Next:
            {
                switch (mode)
                {
                    case CursorMode::Move:
                    {
                        if (cursorPos < I2C_DISP_COLS - 1)
                        {
                            cursorPos++;
                            lcd->SetCursor(cursorPos, 1);
                        }
                        break;
                    }
                    case CursorMode::SelectChar:
                    {
                        selCharPos = (selCharPos + 1) % maxChars;
                        curChar = alphabet[selCharPos];
                        lcd->Write(curChar);
                        lcd->SetCursor(cursorPos, 1);
                        fileNameBuffer[cursorPos] = curChar;
                        break;
                    }
                }
                break;
            }

            case InputResponse::Prev:
            {
                switch (mode)
                {
                    case CursorMode::Move:
                    {
                        if (cursorPos > 0)
                        {
                            cursorPos--;
                            lcd->SetCursor(cursorPos, 1);
                        }
                        break;
                    }

                    case CursorMode::SelectChar:
                    {
                        if (selCharPos == 0)
                        {
                            selCharPos = maxChars;
                        }
                        selCharPos--;
                        curChar = alphabet[selCharPos];
                        lcd->Write(curChar);
                        lcd->SetCursor(cursorPos, 1);
                        fileNameBuffer[cursorPos] = curChar;
                        break;
                    }
                }
                break;
            }

            default:
                break;
        }
    }
}

ErrorCodes RecordSelector::AutoFileName(File& recTap)
{
#define REC_NAME_SIZE 25
    char recName[REC_NAME_SIZE + 1];
    memset(recName, 0, REC_NAME_SIZE + 1);

    for (uint8_t nameIndex = 0; nameIndex < 255; nameIndex++)
    {
        snprintf(recName, REC_NAME_SIZE, "/recorded/rec%03d.tap", nameIndex);
        if (!fileLoader->FileExists(recName))
        {
            ErrorCodes ret = fileLoader->CreateFile(recName, recTap);
            Serial.printf("Creating recording at: %s: ErrorCodes: %d\n", recName, (int) ret);
            if (ret != ErrorCodes::OK)
            {
                lcdUtils->Error(S_RECORDING_FAIL, ret);
            }
            return (ret);
        }
    }
    lcdUtils->Error(S_RECORDING_FAIL, S_GR_256_RECORDINGS);
    return (ErrorCodes::OUT_OF_RANGE);
}

bool RecordSelector::ManualAutoMenu(File& tapFile)
{
    MenuHandler menu(lcdUtils, inputHandler);

    MenuEntry autoManualEntries[] = {
        {MenuEntryType::IndexEntry, S_REC_MODE_AUTO, NULL},
        {MenuEntryType::IndexEntry, S_REC_MODE_MANUAL, NULL},
        {MenuEntryType::IndexEntry, S_EXIT, NULL},
    };
    TheMenu autoManualMenu = {S_SELECT_RECORD_MODE, (MenuEntry*) autoManualEntries, 3, 0, NULL};

    while (true)
    {
        switch (menu.Display(&autoManualMenu))
        {
            // Auto
            case 0:
            {
                return (AutoFileName(tapFile) == ErrorCodes::OK);
            }
            // Manual
            case 1:
            {
                return (ManualFilename(tapFile) == ErrorCodes::OK);
            }
            // Exit (2) or Abort (-1)
            default:
                return (false);
        }
    }
}

void RecordSelector::OnAction()
{
    if (!fileLoader->FileExists("/recorded"))
    {
        if (!fileLoader->CreateDir("/recorded"))
        {
            lcdUtils->Error(S_RECORDING_FAIL, S_CANT_CREATE_DIR);
        }
    }

    File recTap;
    if (ManualAutoMenu(recTap))
    {
        ESP32TapRecorder rec(utilityCollection);
        rec.RecordTap(recTap);
    }
    if (recTap)
    {
        recTap.close();
    }
}
