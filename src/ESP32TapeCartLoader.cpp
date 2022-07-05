#ifdef ESP32
#include "ESP32TapeCartLoader.h"
#include "2-DCube.h"

using namespace TapuinoNext;

#define MAGIC_LOADER_VALUE 0xca65

ESP32TapeCartLoader* ESP32TapeCartLoader::internalClass = NULL;

ESP32TapeCartLoader::ESP32TapeCartLoader(UtilityCollection* utilityCollection)
{
    this->flipBuffer = utilityCollection->flipBuffer;
    this->prgFile = prgFile;
    ESP32TapeCartLoader::internalClass = this;
    loaderMode = false;
    shiftReg = 0;
}

ESP32TapeCartLoader::~ESP32TapeCartLoader()
{
}

void ESP32TapeCartLoader::Init()
{
    HWStartSampling();
}

bool ESP32TapeCartLoader::FastSendByte(uint8_t byte)
{
    portDISABLE_INTERRUPTS();
    digitalWrite(C64_SENSE_PIN, LOW);

    /* wait until write is high or abort if motor is active */
    while (!digitalRead(C64_WRITE_PIN))
    {
        if (digitalRead(C64_MOTOR_PIN))
        {
            digitalWrite(C64_SENSE_PIN, HIGH);
            portENABLE_INTERRUPTS();
            return false;
        }
    }

    uint64_t curTime = esp_timer_get_time();

    /* at 4us: set write to output, send bits 5+4 on sense+write */
    while (esp_timer_get_time() - curTime < 4)
        ;
    pinMode(C64_WRITE_PIN, OUTPUT);
    digitalWrite(C64_SENSE_PIN, byte & (1 << 5));
    digitalWrite(C64_WRITE_PIN, byte & (1 << 4));

    /* at 13us: send bits 7+6 on sense+write */
    while (esp_timer_get_time() - curTime < 13)
        ;
    digitalWrite(C64_SENSE_PIN, byte & (1 << 7));
    digitalWrite(C64_WRITE_PIN, byte & (1 << 6));

    /* at 22us: send bits 1+0 on sense+write */
    while (esp_timer_get_time() - curTime < 22)
        ;
    digitalWrite(C64_SENSE_PIN, byte & (1 << 1));
    digitalWrite(C64_WRITE_PIN, byte & (1 << 0));

    /* at 31us: send bits 3+2 on sense+write */
    while (esp_timer_get_time() - curTime < 31)
        ;
    digitalWrite(C64_SENSE_PIN, byte & (1 << 3));
    digitalWrite(C64_WRITE_PIN, byte & (1 << 2));

    /* at 41us: set write to input again */
    while (esp_timer_get_time() - curTime < 41)
        ;
    pinMode(C64_WRITE_PIN, INPUT_PULLUP); // enables pullup

    /* at 53us: wait until write is low to ensure we don't accidentally re-trigger immediately */
    while (esp_timer_get_time() - curTime < 53)
        ;
    while (digitalRead(C64_WRITE_PIN) && !digitalRead(C64_MOTOR_PIN))
        ;

    digitalWrite(C64_SENSE_PIN, HIGH);
    portENABLE_INTERRUPTS();
    return true;
}

bool ESP32TapeCartLoader::CheckForMode()
{
    int count = 0;
    if (!loaderMode)
    {
        utilityCollection->lcdUtils->Title("Fastloader init");
        utilityCollection->lcdUtils->Status("Waiting...");
        while (count < 10)
        {
            count++;
            delay(100);
        }
    }
    HWStopSampling();
    if (!loaderMode)
    {
        utilityCollection->lcdUtils->Error("FastLoader", "Not detected!");
    }
    return (loaderMode);
}

bool ESP32TapeCartLoader::LoadPRG(File& prgFile)
{
    if (!CheckForMode())
    {
        return false;
    }

    flipBuffer->Reset();

    uint8_t callAddrBuf[2];
    size_t dataSize = prgFile.size();

    prgFile.read(callAddrBuf, 2);
    // remove the call / load address from the size
    dataSize -= 2;

    callAddr = callAddrBuf[0] | ((uint16_t) callAddrBuf[1] << 8);
    uint32_t starterSize = 0;

    if (callAddr == 0x0801)
    {
        static uint8_t basic_starter[] = {
            //            0xfa, 0x07,       // load at $07fa
            0x20, 0x59, 0xa6, // jsr $a659    ; set basic pointer and CLR
            0x4c, 0xae, 0xa7, // jmp $a7ae    ; RUN
            0x00              // $0800 must be zero
        };
        starterSize = sizeof(basic_starter) / sizeof(uint8_t);
        flipBuffer->SetHeader(basic_starter, starterSize);
        callAddr = 0x0801 - starterSize;
    }
    flipBuffer->FillWholeBuffer(prgFile, starterSize);
    loadAddr = callAddr;
    endAddr = loadAddr + dataSize;

    if (!FastSendByte(callAddr & 0xff))
        return false;
    if (!FastSendByte(callAddr >> 8))
        return false;
    if (!FastSendByte(endAddr & 0xff))
        return false;
    if (!FastSendByte(endAddr >> 8))
        return false;
    if (!FastSendByte(loadAddr & 0xff))
        return false;
    if (!FastSendByte(loadAddr >> 8))
        return false;

    /* transmit data */
    while (dataSize > 0)
    {
        dataSize--;
        if (!FastSendByte(flipBuffer->ReadByte()))
            return false;
        flipBuffer->FillBufferIfNeeded(prgFile);
    }

    return true;
}

void ESP32TapeCartLoader::MotorSignalCallback(bool writeHigh)
{
    /* capture write line state */
    shiftReg = (shiftReg << 1) | writeHigh;

    /* check for handshake */
    if (shiftReg == MAGIC_LOADER_VALUE)
    {
        loaderMode = true;
    }
}

void ESP32TapeCartLoader::HWStartSampling()
{
    attachInterrupt(digitalPinToInterrupt(C64_MOTOR_PIN), MotorSignalCallbackStatic, RISING);
}

void ESP32TapeCartLoader::HWStopSampling()
{
    detachInterrupt(digitalPinToInterrupt(C64_MOTOR_PIN));
}

void IRAM_ATTR ESP32TapeCartLoader::MotorSignalCallbackStatic()
{
    bool writeHigh = digitalRead(C64_WRITE_PIN);
    ESP32TapeCartLoader::internalClass->MotorSignalCallback(writeHigh);
}

#endif