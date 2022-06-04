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

#include "ESP32FileLoader.h"
#include "ESP32InputHandler.h"
#include "ESP32TapLoader.h"
#include "ESP32TapRecorder.h"
#include "FS.h"
#include "LCD_HD44780.h"
#include "OLED_1306.h"
#include "Updater.h"

#include "Version.h"

OptionEventHandler optionEventHander;

ESP32FileLoader theFileLoader;

#if I2C_DISP_TYPE == I2C_DISP_HD44780
LCD_HD44780 theLCD(&optionEventHander);
#else
OLED_1306 theLCD(&optionEventHander);
#endif

ESP32InputHandler theInputHandler(&optionEventHander);
LCDUtils lcdUtils(&theLCD);

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

    char buf[51];
    memset(buf, 0, 51);
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

void testEncoderPins()
{
    Serial.println("testEncoderPins");
    pinMode(ROTARY_ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ROTARY_ENCODER_B_PIN, INPUT_PULLUP);

    char buf[50];
    int oldA = digitalRead(ROTARY_ENCODER_A_PIN);
    int oldB = digitalRead(ROTARY_ENCODER_B_PIN);
    strcpy(buf, "00 AB State\n");
    buf[0] = oldA ? '1' : '0';
    buf[1] = oldB ? '1' : '0';
    Serial.print(buf);

    Button encoderButton;
    encoderButton.Init(ROTARY_ENCODER_BUTTON_PIN);

    while (true)
    {
        int a = digitalRead(ROTARY_ENCODER_A_PIN);
        int b = digitalRead(ROTARY_ENCODER_B_PIN);
        if (a != oldA || b != oldB)
        {
            oldA = a;
            oldB = b;

            buf[0] = oldA ? '1' : '0';
            buf[1] = oldB ? '1' : '0';
            Serial.print(buf);
        }
        if (encoderButton.GetState() == ButtonState::CLICKED)
        {
            Serial.println("--------------------------------");
        }
    }
}

bool initTapuino()
{
    Serial.begin(115200);
    Serial.println(S_INIT);
    theLCD.Init(I2C_DISP_ADDR);

    lcdUtils.Title("TapuinoNext");
    Serial.println("TapuinoNext");

    //testEncoderPins();
    theInputHandler.Init();

    char version[I2C_DISP_COLS + 1];
    memset(version, 0, I2C_DISP_COLS + 1);
    snprintf(version, I2C_DISP_COLS, "%s", FW_VERSION);
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
    Updater updater(&lcdUtils, &theInputHandler, &theFileLoader);

    Options options(&theFileLoader, &optionEventHander, &menu, &updater);

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