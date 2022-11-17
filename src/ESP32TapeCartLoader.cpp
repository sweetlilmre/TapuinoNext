#ifdef ESP32
#include "ESP32TapeCartLoader.h"
#include "2-DCube.h"

using namespace TapuinoNext;

#define MAGIC_LOADER_VALUE 0xca65

#pragma pack(push, 1)

static const uint8_t STARTER_CODE[] = {
    0x20, 0x59, 0xa6, // jsr $a659    ; set basic pointer and CLR
    0x4c, 0xae, 0xa7, // jmp $a7ae    ; RUN
    0x00              // $0800 must be zero
};

#define STARTER_CODE_SIZE sizeof(STARTER_CODE)

struct ADDRESS_HEADER
{
    uint16_t callAddr;
    uint16_t endAddr;
    uint16_t loadAddr;
};

#define ADDRESS_HEADER_SIZE sizeof(ADDRESS_HEADER)

union LOADER_HEADER
{
    ADDRESS_HEADER addressHdr;
    uint8_t rawData[ADDRESS_HEADER_SIZE + STARTER_CODE_SIZE];
};

#pragma pack(pop)


ESP32TapeCartLoader* ESP32TapeCartLoader::internalClass = NULL;

ESP32TapeCartLoader::ESP32TapeCartLoader(UtilityCollection* utilityCollection) : utilityCollection(utilityCollection)
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
    shiftReg = 0;
    loaderMode = false;
    HWStartSampling();
}

bool ESP32TapeCartLoader::FastSendByteInner(uint8_t byte)
{
    /* wait until write is high or abort if motor is active */
    while (!digitalRead(C64_WRITE_PIN))
    {
        if (digitalRead(C64_MOTOR_PIN))
        {
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
    pinMode(C64_WRITE_PIN, INPUT_PULLUP); // enable pullup

    /* at 53us: wait until write is low to ensure we don't accidentally re-trigger immediately */
    while (esp_timer_get_time() - curTime < 53)
        ;
    while (digitalRead(C64_WRITE_PIN))
    {
        if (digitalRead(C64_MOTOR_PIN))
        {
            return false;
        }
    }

    return true;
}

bool ESP32TapeCartLoader::FastSendByte(uint8_t byte)
{
    portDISABLE_INTERRUPTS();
    digitalWrite(C64_SENSE_PIN, LOW);
    bool ret = FastSendByteInner(byte);
    digitalWrite(C64_SENSE_PIN, HIGH);
    portENABLE_INTERRUPTS();
    return ret;
}


bool ESP32TapeCartLoader::CheckForMode()
{
    LCDUtils* lcd = utilityCollection->lcdUtils;
    // TODO: fix hard coded strings
    int count = 0;
    if (!loaderMode)
    {
        lcd->Title("Fastloader init");
        lcd->Status("Waiting...");
        while(!loaderMode)
        {
            count++;
            delay(100);
        }
    }
    HWStopSampling();
    return (loaderMode);
}



bool ESP32TapeCartLoader::LoadPRG(File& prgFile)
{
    LCDUtils* lcd = utilityCollection->lcdUtils;

    // TODO: fix hard coded strings
    // if we didn't get the handshake then something went wrong with the TapeCart loader
    
    if (!CheckForMode())
    {
        lcd->Error("FastLoader", "Not detected!");
        return false;
    }
    lcd->Status("");
    lcd->Title("Loading PRG...");

    // clean up the flipBuffer
    flipBuffer->Reset();

    size_t dataSize = prgFile.size();

    // TODO: what if the file size is abnormally large?

    LOADER_HEADER ldrHdr;

    prgFile.read((uint8_t*) &ldrHdr.addressHdr.callAddr, 2);
    // skip the call / load address
    dataSize -= 2;

    // the header we write into the flipBuffer will, at a minimum, be 6 bytes long:
    //    callAddr
    //    endAddr
    //    loadAddr
    uint32_t fbHeaderSize = ADDRESS_HEADER_SIZE;

    // end address is: call address + remaining length of the PRG file (total PRG length - 2)
    ldrHdr.addressHdr.endAddr = ldrHdr.addressHdr.callAddr + dataSize;

    // do we need a starter?
    if (ldrHdr.addressHdr.callAddr == 0x0801)
    {
        // move the call address back to point to the prepended basic starter code
        ldrHdr.addressHdr.callAddr -= STARTER_CODE_SIZE;
        // copy in the starter code after the address header
        memcpy(&ldrHdr.rawData[ADDRESS_HEADER_SIZE], STARTER_CODE, STARTER_CODE_SIZE);
        // increase the size of the flipBuffer header to include the starter
        fbHeaderSize += STARTER_CODE_SIZE;
    }

    // load address will be the same as the call address
    ldrHdr.addressHdr.loadAddr = ldrHdr.addressHdr.callAddr;

    // adjust the total amount of data to be transmitted (address header and possible starter code)
    dataSize += fbHeaderSize;

    // write out the address header and possible starter code
    if (ErrorCodes::OK != flipBuffer->CopyBlock(ldrHdr.rawData, fbHeaderSize))
    {
        return false;
    }
    // fill the rest of the flipBuffer from the PRG
    flipBuffer->FillWholeBuffer(prgFile, fbHeaderSize);

    // send the whole lot (including address header block and possible starter code)
    while (dataSize > 0)
    {
        dataSize--;
        if (!FastSendByte(flipBuffer->ReadByte()))
            return false;
        flipBuffer->FillBufferIfNeeded(prgFile);
    }

    return true;
}

void ESP32TapeCartLoader::MotorSignalCallback(int writeHigh)
{
    /* capture write line state */
    shiftReg = (shiftReg << 1) | writeHigh;

    /* check for handshake */
    if (shiftReg == MAGIC_LOADER_VALUE)
    {
        digitalWrite(C64_SENSE_PIN, HIGH);
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
    int writeHigh = digitalRead(C64_WRITE_PIN);
    ESP32TapeCartLoader::internalClass->MotorSignalCallback(writeHigh);
}

#endif