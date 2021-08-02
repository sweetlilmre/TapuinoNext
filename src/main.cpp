#ifdef ESP32

#include <iostream>
#include <string>
#include <vector>

#include "FS.h"
#include "Lang.h"
#include "LoadSelector.h"
#include "Menu.h"
#include "Options.h"
#include "RecordSelector.h"
#include "SD_MMC.h"
#include "config.h"
#include "OptionEventHandler.h"
using namespace std;
using namespace TapuinoNext;

#ifdef ESP32
#include "ESP32FileLoader.h"
#include "ESP32InputHandler.h"
#include "ESP32TapLoader.h"
#include "ESP32TapRecorder.h"
#include "FS.h"
#include "LCD_HD44780.h"
#include "OLED_1306.h"

OptionEventHandler optionEventHander;

ESP32FileLoader theFileLoader;

#if I2C_DISP_TYPE == I2C_DISP_HD44780
LCD_HD44780 theLCD(&optionEventHander);
#else
OLED_1306 theLCD;
#endif

ESP32InputHandler theInputHandler(&optionEventHander);
LCDUtils lcdUtils(&theLCD);
#endif

void testInputHander()
{
    while (true)
    {
        switch (theInputHandler.GetInput())
        {
            case InputResponse::Next:
                Serial.println("next");
                break;
            case InputResponse::Prev:
                Serial.println("prev");
                break;
            case InputResponse::Select:
                Serial.println("Select");
                break;
            case InputResponse::Abort:
                Serial.println("Abort");
                break;
            default:
                break;
        }
    }
}

void testPins()
{
    pinMode(C64_SENSE_PIN, OUTPUT);
    digitalWrite(C64_SENSE_PIN, LOW);

    pinMode(C64_MOTOR_PIN, INPUT_PULLDOWN);

    pinMode(C64_READ_PIN, OUTPUT);
    digitalWrite(C64_READ_PIN, LOW);

    pinMode(C64_WRITE_PIN, INPUT_PULLDOWN);

    char buf[50];
    while (true)
    {
        int w = digitalRead(C64_WRITE_PIN);
        int m = digitalRead(C64_MOTOR_PIN);
        int s = digitalRead(C64_SENSE_PIN);
        int r = digitalRead(C64_READ_PIN);

        snprintf(buf, 50, "W-%c M-%c S-%c R-%c", w ? 'H' : 'L', m ? 'H' : 'L', s ? 'H' : 'L', r ? 'H' : 'L');
        lcdUtils.Title(buf);

        delay(500);
    }
}

#define TAPUINO_MAJOR_VERSION 0
#define TAPUINO_MINOR_VERSION 1
#define TAPUINO_BUILD_VERSION 0

bool initTapuino()
{
    Serial.begin(115200);
    Serial.println(S_INIT);
    theLCD.Init(I2C_DISP_ADDR);

    lcdUtils.Title("TapuinoNext");
    Serial.println("TapuinoNext");
    char version[I2C_DISP_COLS + 1];
    snprintf(version, I2C_DISP_COLS + 1, "V: %d.%d.%d", TAPUINO_MAJOR_VERSION, TAPUINO_MINOR_VERSION, TAPUINO_BUILD_VERSION);
    lcdUtils.Status(version);
    Serial.println(version);
    delay(2000);

    lcdUtils.Status(S_INIT);
    Serial.println("Starting SD init");

    if (!theFileLoader.Init())
    {
        Serial.println(S_INIT_FAILED);
        lcdUtils.Status(S_INIT_FAILED);
        return false;
    }
    Serial.println(S_INIT_OK);
    lcdUtils.Status(S_INIT_OK);
    delay(1000);
    return (true);
}

MenuEntry mainMenuEntries[] = {
    {MenuEntryType::IndexEntry, S_INSERT_TAP, NULL},
    {MenuEntryType::IndexEntry, S_RECORD_TAP, NULL},
    {MenuEntryType::IndexEntry, S_OPTIONS, NULL},
};

TheMenu mainMenu = {S_MAIN_MENU, (MenuEntry*) mainMenuEntries, 3, 0, NULL};

void setup()
{
    if (!initTapuino())
        return;

    MenuHandler menu(&lcdUtils, &theInputHandler);

    Options options(&theFileLoader, &optionEventHander, &menu);

    UtilityCollection utilityCollection(&lcdUtils, &theInputHandler, &theFileLoader, &options);
    LoadSelector lsel(&utilityCollection);
    RecordSelector rsel(&utilityCollection);
    options.LoadOptions();

    while (true)
    {
        int index = menu.Display(&mainMenu);
        switch (index)
        {
            case 0:
            {
                lsel.OnAction();
                break;
            }
            case 1:
            {
                rsel.OnAction();
                break;
            }
            case 2:
            {
                options.OnAction();
                break;
            }
        }
    }
}

void loop()
{
}

#endif