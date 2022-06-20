#include "TapRecorder.h"
#include "Lang.h"
#include <Arduino.h>

using namespace std;

using namespace TapuinoNext;

TapRecorder::TapRecorder(UtilityCollection* utilityCollection, uint32_t bufferSize) : TapBase(utilityCollection, bufferSize)
{
    isSampling = false;
}

TapRecorder::~TapRecorder()
{
}

inline void TapRecorder::WriteNextByte(uint8_t nextByte)
{
    flipBuffer->WriteByte(nextByte);
    tapInfo.position++;
}

void TapRecorder::CalcTapData(uint32_t signalTime)
{
    /*
    if (tapInfo.version == 2)
    {
        // for TAP format 2 (half-wave) each half of the signal is represented indivudually and so is returned as double the length of a version 0 or 1 value
        // so that the 2 Mhz counter doesn't have to do the work. The alternative would be to halve the version 0 or 1 values.
        signalTime >>= 1;
    }
    */
    // TODO: check this for accuracy, signal time is uS, cycle time is machine dependent.
    //       however counter calculation is definitely uS based, both rec and play use uS so perhaps cycles is not the correct
    //       name for this.
    tapInfo.cycles += signalTime;

    uint32_t tapData = (uint32_t) ((double) signalTime / cycleMult8);
    if (tapData < 256)
    {
        WriteNextByte((uint8_t) tapData);
    }
    else
    {
        // Only TAP version 1 files are supported.
        // TODO: implement Version 2 (C16 half-wave)
        // TAP version 0 will NOT be supported

        WriteNextByte(0);
        uint32_t tapData = (uint32_t) ((double) signalTime / cycleMultRaw);
        WriteNextByte((uint8_t) tapData);
        WriteNextByte((uint8_t) (tapData >> 8));
        WriteNextByte((uint8_t) (tapData >> 16));
    }
    tapInfo.length = tapInfo.position;
}

void TapRecorder::StartSampling()
{
    if (!isSampling)
    {
        isSampling = true;
        processSignal = true;
        HWStartSampling();
        // tell the C64 that play has been pressed
        digitalWrite(C64_SENSE_PIN, LOW);
    }
    else
    {
        Serial.println("TapRecorder::StartSampling() called while already sampling!");
    }
}

void TapRecorder::StopSampling()
{
    if (isSampling)
    {
        isSampling = false;
        // prevent any further buffer processing
        processSignal = false;
        // shutdown the hardware timer
        HWStopSampling();

        // tell the C64 that stop has been pressed
        digitalWrite(C64_SENSE_PIN, HIGH);
    }
    else
    {
        Serial.println("TapRecorder::StopSampling() called while not sampling!");
    }
}

ErrorCodes TapRecorder::CreateTap(File tapFile)
{
    uint32_t tap_magic[TAP_HEADER_MAGIC_LENGTH / 4];
    memset(&tapInfo, 0, sizeof(tapInfo));

    flipBuffer->Reset();
    tapFile.seek(0);

    // TODO: C16 recording is not properly implemented as yet.
    // The user can set the value to the C16 machine type, but half-wave recording WILL NOT OCCUR!!!
    MACHINE_TYPE machineType = (MACHINE_TYPE) options->machineType.GetValue();

    tap_magic[0] = machineType == MACHINE_TYPE::C16 ? TAP_MAGIC_C16 : TAP_MAGIC_C64;
    // check the post fix for "TAPE-RAW", use a 4-byte magic trick
    tap_magic[1] = TAP_MAGIC_POSTFIX1;
    tap_magic[2] = TAP_MAGIC_POSTFIX2;

    tapInfo.version = machineType == MACHINE_TYPE::C16 ? TAP_HEADER_VERSION_2 : TAP_HEADER_VERSION_1;
    tapInfo.video = (uint8_t) (options->ntscPAL.GetValue() ? VIDEO_MODE::NSTC : VIDEO_MODE::PAL);
    tapInfo.platform = (uint8_t) machineType;

    size_t written = tapFile.write((uint8_t*) &tap_magic, TAP_HEADER_MAGIC_LENGTH);
    if (written != TAP_HEADER_MAGIC_LENGTH)
    {
        Serial.printf("unable to write TAP magic! Length: %d\n", written);
        return ErrorCodes::FILE_WRITE_ERROR;
    }

    written = tapFile.write((uint8_t*) &tapInfo, TAP_HEADER_DATA_LENGTH);
    if (written != TAP_HEADER_DATA_LENGTH)
    {
        Serial.printf("unable to write TAP header! Length: %d\n", written);
        return ErrorCodes::FILE_WRITE_ERROR;
    }

    SetupCycleTiming();

    return (ErrorCodes::OK);
}

bool TapRecorder::InRecordMenu(File tapFile)
{
    MenuHandler menu(lcdUtils, inputHandler);

    MenuEntry inRecordEntries[] = {
        {MenuEntryType::IndexEntry, S_CONTINUE, NULL},
        {MenuEntryType::IndexEntry, S_EXIT, NULL},
    };
    TheMenu inPlayMenu = {S_RECORD_MENU, (MenuEntry*) inRecordEntries, 2, 0, NULL};

    switch (menu.Display(&inPlayMenu))
    {
        // Play
        case 0:
        {
            return (true);
        }
        // Exit (1) or Abort (-1)
        default:
            return (false);
    }
}

void TapRecorder::FinalizeRecording(File tapFile)
{
    flipBuffer->FlushBufferFinal(tapFile);
    tapFile.seek(TAP_HEADER_MAGIC_LENGTH);
    tapFile.write((uint8_t*) &tapInfo, TAP_HEADER_DATA_LENGTH);
}

void TapRecorder::RecordTap(File tapFile)
{
    ErrorCodes ret = CreateTap(tapFile);
    if (ret != ErrorCodes::OK)
    {
        lcdUtils->Error(S_CREATE_FAILED, "");
        Serial.printf("Unable to create: %s, %d\n", tapFile.name(), (int) ret);
        return;
    };

    lcdUtils->Title(S_SELECT_TO_START_REC);
    lcdUtils->ShowFile(tapFile.name(), false);

    while (inputHandler->GetInput() != InputResponse::Select)
    {
        delay(10);
    }

    delay(1000);
    lcdUtils->Title(S_RECORDING);

    StartSampling();

    while (true)
    {
        motorOn = digitalRead(C64_MOTOR_PIN);
        flipBuffer->FlushBufferIfNeeded(tapFile);

        // (uint16_t) (DS_G * (sqrt((tapInfo.cycles / 1000000.0 * (DS_V_PLAY / DS_D / PI)) + ((DS_R * DS_R) / (DS_D * DS_D))) - (DS_R / DS_D)));
        tapInfo.counterActual = CYCLES_TO_COUNTER(tapInfo.cycles);
        lcdUtils->PlayUI(motorOn, tapInfo.counterActual, 200);

        InputResponse resp = inputHandler->GetInput();

        if (resp == InputResponse::Select || resp == InputResponse::Abort)
        {
            StopSampling();
            if (resp == InputResponse::Abort)
            {
                FinalizeRecording(tapFile);
                return;
            }
            if (!InRecordMenu(tapFile))
            {
                Serial.println("exiting recording");
                FinalizeRecording(tapFile);
                return;
            }
            lcdUtils->Title(S_RECORDING);
            lcdUtils->ShowFile(tapFile.name(), false);
            StartSampling();
        }
    }
}
