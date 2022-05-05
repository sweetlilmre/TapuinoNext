#include "LCDUtils.h"
#include "Lang.h"
#include "config.h"
#include <Arduino.h>
#include <FS.h>
#include <cstring>

using namespace TapuinoNext;

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

LCDUtils::LCDUtils(LCD* lcd) : lcd(lcd)
{
    lastTick = millis();
    lastHoldTick = 0;
    fileNameBuf[0] = 0;
    fileNameScroll = NULL;
}

LCD* LCDUtils::GetLCD()
{
    return (lcd);
}

void LCDUtils::Title(const char* msg, char indicator)
{
    Line(0, msg, indicator);
}

void LCDUtils::Title(const char* msg)
{
    Line(0, msg, 0);
}

void LCDUtils::Status(const char* msg, char indicator)
{
    Line(1, msg, indicator);
}

void LCDUtils::Status(const char* msg)
{
    Line(1, msg, 0);
}

void LCDUtils::Error(const char* title, ErrorCodes errorCode)
{
    switch (errorCode)
    {
        case ErrorCodes::FILE_ERROR:
            Error(title, S_FILE_ERROR);
            break;
        case ErrorCodes::FILE_NOT_FOUND:
            Error(title, S_FILE_NOT_FOUND);
            break;
        case ErrorCodes::FILE_WRITE_ERROR:
            Error(title, S_FILE_WRITE_ERROR);
            break;
        case ErrorCodes::FILE_EXISTS_ERROR:
            Error(title, S_FILE_EXISTS_ERROR);
            break;
        case ErrorCodes::INVALID_TAP_FILE:
            Error(title, S_INVALID_TAP);
            break;
        case ErrorCodes::UNKNOWN_TAP_FORMAT:
            Error(title, S_UNKNOWN_TAP);
            break;
        case ErrorCodes::OUT_OF_MEMORY:
            Error(title, S_OUT_OF_MEMORY);
            break;
        case ErrorCodes::INVALID_COUNTER_POS:
            Error(title, S_COUNTER_TOO_BIG);
            break;
        default:
            Error(title, "");
            break;
    }
}

void LCDUtils::Error(const char* title, const char* msg)
{
    Title(title);
    Status(msg);
    for (int i = 0; i < 10; i++)
    {
        lcd->SetCursor(I2C_DISP_COLS - 1, 0);
        lcd->Write(Spinner());
        delay(100);
    }
}

void LCDUtils::PlayUI(bool motor, uint16_t counter, uint32_t tickerTime)
{
    if (millis() - lastTick < tickerTime)
    {
        return;
    }

    lastTick = millis();
    char buf[8];
    memset(buf, 32, 8);

    lcd->SetCursor(I2C_DISP_COLS - 6, 0);
    snprintf(buf, 7, "%03u %c%c", counter % 1000u, motor ? 'M' : 'm', Spinner());
    lcd->Print(buf);
}

void LCDUtils::ShowFile(const char* fileName, bool title, bool isDirectory)
{
    const char* name = strrchr(fileName, '/');
    if (name != NULL)
    {
        name++;
    }
    if (name == NULL || *name == 0)
    {
        name = fileName;
    }

    int len = strlen(name);
    strncpy(fileNameBuf, name, MIN(len, 255) + 1);

    if (title)
    {
        Title(fileNameBuf);
    }
    else
    {
        Status(fileNameBuf, isDirectory ? DIRECTORY_INDICATOR : 0);
        fileNameScroll = fileNameBuf;
        lastTick = millis();
        lastHoldTick = 0;
    }
}

void LCDUtils::ShowFile(const char* fileName, bool title)
{
    ShowFile(fileName, title, false);
}

void LCDUtils::ScrollFileName(uint32_t tickerTime, uint32_t tickerHoldTime, bool isDirectory)
{
    int len = strlen(fileNameBuf);
    // no scroll needed OMG edge cases!
    // 1. don't scroll if the string is LESS than the display length (this caters for leaving a blank space for the dir indicator)
    // 2. don't scroll if the string is EXACTLY the display length and IS NOT a dir.
    if (len < I2C_DISP_COLS || (len == I2C_DISP_COLS && !isDirectory))
    {
        return;
    }

    // sanity check and wait at the minimum timer between updates: tickerTime
    if ((fileNameScroll == NULL) || (millis() - lastTick < tickerTime))
    {
        return;
    }

    lastTick = millis();

    if (lastHoldTick == 0)
    {
        lastHoldTick = lastTick;
    }

    if (lastTick - lastHoldTick < tickerHoldTime)
    {
        return;
    }

    len = I2C_DISP_COLS;
    // don't scroll an extra character if the file is not a dir (i.e. there is no dir marker at the end)
    // see the < comparison below. So we hack this by making the length we're comparing to be 1 greater.
    if (!isDirectory)
    {
        len++;
    }

    // if the scrolled text length is now less than the displayable length, perform the end hold logic
    if (strlen(fileNameScroll) < len)
    {
        // hold for tickerHoldTime. The << 1 is because the hold check above needs to fall through to here,
        // so we update lastHold tick below to be 1ms longer than tickerHoldTime, causing the previous hold check to fall through
        if (lastTick - lastHoldTick < tickerHoldTime << 1)
        {
            return;
        }
        // otherwise reset everything, so that we will hold and the string is reset to the first char position
        lastHoldTick = 0;
        fileNameScroll = fileNameBuf;
    }
    else
    {
        // we are still scrolling so increment the position in the string
        // and update lastHoldTick to be 1ms > tickerHoldTime in preparation for the
        // end hold logic above.
        fileNameScroll++;
        lastHoldTick = lastTick - tickerHoldTime - 1;
    }

    Status(fileNameScroll, isDirectory ? DIRECTORY_INDICATOR : 0);
}

char LCDUtils::Spinner()
{
    static uint8_t indicators[] = {'|', '/', '-', '\\'};
    static uint8_t pos = 0;
    char ret = indicators[pos++];
    if (pos > 3)
        pos = 0;
    return (ret);
}

void LCDUtils::Line(uint8_t line, const char* msg, char indicator)
{
    char buf[I2C_DISP_COLS + 1];
    buf[I2C_DISP_COLS] = 0;
    int len = strlen(msg);
    memset(buf, 32, I2C_DISP_COLS);
    strncpy(buf, msg, MIN(I2C_DISP_COLS, len));
    if (indicator != 0)
    {
        buf[I2C_DISP_COLS - 1] = indicator;
    }
    lcd->SetCursor(0, line);
    lcd->Print(buf);
}
